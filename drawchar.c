#include <ft2build.h>
#include FT_FREETYPE_H
#include <math.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include "thermalview.h"

/*フォントのデータを保存するグローバルデータ*/
static FT_Library library;		//フォントライブラリ
static FT_Face    face;		//フォントフェイス
static FT_GlyphSlot slot;		//フォントスロット
int fontload=0;			//フォントのロードフラグ

#define SIZE_OF_ARRAY(array) (sizeof(array)/sizeof(array[0]))

/*************************
 * マルチバイト文字の文字数を数える
 * 引数　buf:  文字数を数えたい文字列
 * 戻り値　文字数
 */
int strwlen(char *buf)
{
    // LC_CTYPE をネイティブロケールに変更
    if( setlocale( LC_CTYPE, "" ) == NULL ){
        fprintf(stderr, "do not set locale.\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    int count = 0;
    while( buf[i] != '\0' ){
        int res = mblen( &buf[i], MB_CUR_MAX );
        if( res < 0 ){
            fprintf(stderr, "Contains illegal characters.\n");
            exit(EXIT_FAILURE);
        }

        i += res;
        count++;
    }

    return count;
}

/*************************
 * TrueType fontsの設定
 * 引数　fontsfile:   TrueType fontsのファイル名
 * 戻り値　なし
 */
void setfonts(char fontsfile[])
{
	int error;
	
	if(fontload != 0) {
		error = FT_Done_Face( face );
		if(error) {
			fprintf(stderr,"error FT_Done_Face\n");
			exit(EXIT_FAILURE);
		}
		error = FT_Done_FreeType( library );
		if(error) {
			fprintf(stderr,"error FT_Done_FreeType\n");
			exit(EXIT_FAILURE);
		}
	}
	// FreeTypeの初期化とTrueTypeフォントの読み込み
    error = FT_Init_FreeType( &library );
    if(error) {
		fprintf(stderr,"error FT_Init_FreeType\n");
		exit(EXIT_FAILURE);
    }
    error = FT_New_Face( library, fontsfile, 0, &face );
    if(error == FT_Err_Unknown_File_Format) {
		fprintf(stderr,"unsupport fonts format\n");
		exit(EXIT_FAILURE);
    } else if(error) {
		fprintf(stderr,"fonts file not found\n");
		exit(EXIT_FAILURE);
    }
    slot = face->glyph;
    fontload = 1;
}

/*************************
 * 文字列の表示
 * 引数　draw_x:    表示するx座標
 *      draw_y:　　 表示するy座標
 *      angle:     表示する角度
 *      fontssize: 文字の大きさ
 *      text:　　　　表示する文字列 \nで改行する
 * 戻り値　なし
 */
void drawchar(int win,int draw_x,int draw_y,double angle,int fontssize,char text[],int r,int g,int b)
{
    int error;
    wchar_t *ws; /* ワイド文字保存用 */
    int num_chars = strwlen(text);
    
    ws = (wchar_t *)malloc( num_chars*sizeof(wchar_t) );
    if( ws==NULL ) {
		fprintf(stderr, "malloc error\n");
		exit( EXIT_FAILURE );
    }		
    int len = mbstowcs( ws, text, num_chars );
    if( len == -1 ){
		fprintf(stderr, "mbstowcs error\n");
		exit( EXIT_FAILURE );
    }

    error = FT_Set_Pixel_Sizes( face, 0, fontssize ); //ピクセル単位でサイズ指定
    //error = FT_Set_Char_Size( face, 0, 16 * 64, 300, 300); //ポイント単位でサイズ指定
    if(error) {
		fprintf(stderr,"FT_Set_Pixel_Sizes error\n");
		exit(EXIT_FAILURE);
    }
    
    FT_Matrix     matrix;              /* transformation matrix */
    FT_Vector     pen;                 /* untransformed origin */
	int line=0;
	/* set up matrix */
	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );
	pen.x = draw_x*64;
	pen.y = (-draw_y)*64;
	for (int n = 0;n < num_chars; n++ ){
	    FT_Bitmap bitmap;
	    FT_Set_Transform( face, &matrix, &pen );
	    switch(ws[n]) {
		case '\r':
		    pen.x  =  draw_x*64 + cos(angle-M_PI/2) * line * (face->size->metrics.height);
		    pen.y  = -draw_y*64 + sin(angle-M_PI/2) * line * (face->size->metrics.height);
		    break;
		case '\t': /*tabは正しく計算していません */
		    pen.x  = pen.x - sin(angle-M_PI/2)*(face->size->metrics.max_advance);
		    pen.y  = pen.y + cos(angle-M_PI/2)*(face->size->metrics.max_advance);
		    break;
		case '\n':
		    line++;
		    pen.x  =  draw_x*64 + cos(angle-M_PI/2) * line * (face->size->metrics.height);
		    pen.y  = -draw_y*64 + sin(angle-M_PI/2) * line * (face->size->metrics.height);
		    //(face->size->metrics.height>>6) 1文字の最大高さ　次の行のベースライン 固定小数点　26.6 bit
		    break;
		default:
		    // n文字目の文字をビットマップ化
		    error = FT_Load_Char( face, ws[n], FT_LOAD_RENDER |FT_LOAD_MONOCHROME );// 横書き アンチエリアシングなし
		    if(error) {
				fprintf(stderr,"FT_Load_Char error\n");
				exit(EXIT_FAILURE);
		    }
		    bitmap = slot->bitmap;

			uint8_t *imgbuf = (uint8_t *)malloc(bitmap.rows * bitmap.width * 4);
			memset(imgbuf, 0x00, bitmap.rows * bitmap.width * 4);
		    for( int loopy = 0; loopy < bitmap.rows ; loopy++){
				for(int byte_index=0;byte_index<bitmap.pitch;byte_index++) {
					int byte_value,rowstart,end_loop,num_bits_done;
					byte_value = bitmap.buffer[loopy*bitmap.pitch+byte_index];
					num_bits_done = byte_index*8;
					rowstart = loopy*bitmap.width + byte_index*8;
					if(8 > (bitmap.width - num_bits_done)) {
						end_loop = bitmap.width - num_bits_done;
					} else {
						end_loop = 8;
					}
					for(int bit_index=0;bit_index<end_loop;bit_index++) {
						int bit = byte_value & (1<<(7-bit_index));
						if(bit!=0) {
							int x = (bit_index+rowstart)%bitmap.width  + slot->bitmap_left;
							int y = (bit_index+rowstart)/bitmap.width - slot->bitmap_top;
							imgbuf[(bit_index+rowstart)*4 + 0] = 0xff;
							imgbuf[(bit_index+rowstart)*4 + 1] = r;
							imgbuf[(bit_index+rowstart)*4 + 2] = g;
							imgbuf[(bit_index+rowstart)*4 + 3] = b;
							//_pset(x,y); /* 点を描く */
						}
					}
				}
		    }
		    gputimage(win,slot->bitmap_left,slot->bitmap_top,imgbuf,bitmap.width,bitmap.rows,1);
		    pen.x += slot->advance.x ;
		    pen.y += slot->advance.y ;
			free(imgbuf);
	    }
	}
	free(ws);
}

