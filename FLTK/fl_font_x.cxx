//
// "$Id: fl_font_x.cxx,v 1.64 2012/11/21 08:51:38 yxy Exp $"
//
// Standard X11 font selection code for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2002 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//
#include <stdio.h>
#include <stdlib.h>
#include<iconv.h>
#include<string.h>
#include <FL/fl_font_x.H>


#define VITAS
unsigned * ExtractStrDiplay( unsigned* dispfont , unsigned* string , int len ) ;
unsigned RussiaCodeTransform(unsigned long testcode) ;
unsigned JapenCodeTransform(unsigned long testcode) ;
unsigned KoreaCodeTransform(unsigned long testcode) ;

#define dbout(str)				printf((str));printf("\n")

int Set_HZK14_Matrice(unsigned char* toBuffer)   //将中文字模库HZK14的内容，读入内存
{
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;
    
    /* 若要一个byte不漏地读入整个文件，只能采用二进制方式打开 */ 
    pFile = fopen ("HZK14", "rb" );
    if (pFile==NULL)
    {
        printf("Can not Open File");
        return 0;
    }
    /* 获取文件大小 */     
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    
    rewind (pFile);  

    /* 将文件拷贝到buffer中 */
    result = fread (toBuffer,1,lSize,pFile);
    if (result != lSize)
    {
        printf ("Reading error");
        return 0;
    }
    /* 关闭文件 */
    fclose (pFile); 
    return 0;
}
     unsigned long Utf8_to_Unicode (const char * str)
        {
            unsigned char uChar[2];
            uChar[0] = ((str[0] & 0x0f) <<4) + ((str[1] >>2) & 0x0f);
            uChar[1] = ((str[1] & 0x03) <<6) + (str[2] * 0x3f);
            
            return (unsigned long)(uChar[0] *256 + uChar[1]);            
        }         
        int Is_Chinese_Word(unsigned long uValue)         // 判断条件为中文的范围为4e00~9FA5
        {   
            if((uValue >= 0x4e00) && (uValue <= 0x9FA5))
               //||(uValue >= 0x2E80 && uValue <= 0xA4CF))
                // ||(uValue >= 0xFF00 && uValue <= 0xFFEF))
            {
                return 1;
            }
            else
            {
               return 0; 
            }              
        }

        //代码转换:从一种编码转为另一种编码
        int code_convert(char *from_charset,char *to_charset, char *inbuf,size_t inlen,char *outbuf,size_t outlen)
        {
           iconv_t cd;            
           char **pin = &inbuf;
           char **pout = &outbuf;
           cd = iconv_open(to_charset,from_charset);
           if (cd == 0) return -1;
           memset(outbuf,0,outlen);
           if (iconv(cd,pin,&inlen,pout,&outlen) ==-1) 
                return -1;
           iconv_close(cd);
           return 0;
        }

        //UNICODE码转为GB2312码
         int Utf8_To_GB2312(char *inbuf,int inlen,char *outbuf,int outlen)
         {
            return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
               
         }

Fl_FontSize* fl_fontsize;

	Fl_FontSize::Fl_FontSize(const char* name) {
		font = XLoadQueryFont(fl_display, name);
		if (!font) {
			Fl::warning("bad font: %s", name);
			font = XLoadQueryFont(fl_display, "fixed"); // if fixed fails we crash
		}
#  if HAVE_GL
		listbase = 0;
#  endif
	}

	Fl_FontSize::~Fl_FontSize() {
#  if HAVE_GL
		// Delete list created by gl_draw().  This is not done by this code
		// as it will link in GL unnecessarily.  There should be some kind
		// of "free" routine pointer, or a subclass?
		// if (listbase) {
		//  int base = font->min_char_or_byte2;
		//  int size = font->max_char_or_byte2-base+1;
		//  int base = 0; int size = 256;
		//  glDeleteLists(listbase+base,size);
		// }
#  endif
		if (this == fl_fontsize) fl_fontsize = 0;
		XFreeFont(fl_display, font);
	}

	////////////////////////////////////////////////////////////////

	// WARNING: if you add to this table, you must redefine FL_FREE_FONT
	// in Enumerations.H & recompile!!
	static Fl_Fontdesc built_in_table[] = {
		{"-*-helvetica-medium-r-normal--*"},
		{"-*-helvetica-bold-r-normal--*"},
		{"-*-helvetica-medium-o-normal--*"},
		{"-*-helvetica-bold-o-normal--*"},
		{"-*-courier-medium-r-normal--*"},
		{"-*-courier-bold-r-normal--*"},
		{"-*-courier-medium-o-normal--*"},
		{"-*-courier-bold-o-normal--*"},
		{"-*-times-medium-r-normal--*"},
		{"-*-times-bold-r-normal--*"},
		{"-*-times-medium-i-normal--*"},
		{"-*-times-bold-i-normal--*"},
		{"-*-symbol-*"},
		{"-*-lucidatypewriter-medium-r-normal-sans-*"},
		{"-*-lucidatypewriter-bold-r-normal-sans-*"},
		{"-*-*zapf dingbats-*"}
	};


	Fl_Fontdesc* fl_fonts = built_in_table;

#define MAXSIZE 32767

	// return dash number N, or pointer to ending null if none:
	const char* fl_font_word(const char* p, int n) {
		while (*p) {if (*p=='-') {if (!--n) break;} p++;}
		return p;
	}

	// return a pointer to a number we think is "point size":
	char* fl_find_fontsize(char* name) {
		char* c = name;
		// for standard x font names, try after 7th dash:
		if (*c == '-') {
			c = (char*)fl_font_word(c,7);
			if (*c++ && isdigit(*c)) return c;
			return 0; // malformed x font name?
		}
		char* r = 0;
		// find last set of digits:
		for (c++;* c; c++)
			if (isdigit(*c) && !isdigit(*(c-1))) r = c;
		return r;
	}

	const char* fl_encoding = "iso8859-1";

	// return true if this matches fl_encoding:
	int fl_correct_encoding(const char* name) {
		if (*name != '-') return 0;
		const char* c = fl_font_word(name,13);
		return (*c++ && !strcmp(c,fl_encoding));
	}

	// locate or create an Fl_FontSize for a given Fl_Fontdesc and size:
	static Fl_FontSize* find(int fnum, int size) {
		Fl_Fontdesc* s = fl_fonts+fnum;
		if (!s->name) s = fl_fonts; // use font 0 if still undefined
		Fl_FontSize* f;
		for (f = s->first; f; f = f->next)
			if (f->minsize <= size && f->maxsize >= size) return f;
		fl_open_display();
		if (!s->xlist) {
			s->xlist = XListFonts(fl_display, s->name, 100, &(s->n));
			if (!s->xlist) {	// use fixed if no matching font...
				s->first = new Fl_FontSize("fixed");
				s->first->minsize = 0;
				s->first->maxsize = 32767;
				return s->first;
			}
		}
		// search for largest <= font size:
		char* name = s->xlist[0]; int ptsize = 0;	// best one found so far
		int matchedlength = 32767;
		char namebuffer[1024];	// holds scalable font name
		int found_encoding = 0;
		int m = s->n; if (m<0) m = -m;
		for (int n=0; n < m; n++) {

			char* thisname = s->xlist[n];
			if (fl_correct_encoding(thisname)) {
				if (!found_encoding) ptsize = 0; // force it to choose this
				found_encoding = 1;
			} else {
				if (found_encoding) continue;
			}
			char* c = fl_find_fontsize(thisname);
			int thissize = c ? atoi(c) : MAXSIZE;
			int thislength = strlen(thisname);
			if (thissize == size && thislength < matchedlength) {
				// exact match, use it:
				name = thisname;
				ptsize = size;
				matchedlength = thislength;
			} else if (!thissize && ptsize!=size) {
				// whoa!  A scalable font!  Use unless exact match found:
				int l = c-thisname;
				memcpy(namebuffer,thisname,l);
				l += sprintf(namebuffer+l,"%d",size);
				while (*c == '0') c++;
				strcpy(namebuffer+l,c);
				name = namebuffer;
				ptsize = size;
			} else if (!ptsize ||	// no fonts yet
				thissize < ptsize && ptsize > size || // current font too big
				thissize > ptsize && thissize <= size // current too small
				) {
					name = thisname; ptsize = thissize;
					matchedlength = thislength;
				}
		}

		if (ptsize != size) { // see if we already found this unscalable font:
			for (f = s->first; f; f = f->next) {
				if (f->minsize <= ptsize && f->maxsize >= ptsize) {
					if (f->minsize > size) f->minsize = size;
					if (f->maxsize < size) f->maxsize = size;
					return f;
				}
			}
		}

		// okay, we definately have some name, make the font:
		f = new Fl_FontSize(name);
		if (ptsize < size) {f->minsize = ptsize; f->maxsize = size;}
		else {f->minsize = size; f->maxsize = ptsize;}
		f->next = s->first;
		s->first = f;
		return f;

	}

	////////////////////////////////////////////////////////////////
	// Public interface:

	int fl_font_;
	int fl_size_;
	XFontStruct* fl_xfont;
	static GC font_gc;
	int fl_font_style;		//ccz_07_05_12

	void fl_font(int fnum, int size) {
		if (fnum == fl_font_ && size == fl_size_) return;
		fl_font_ = fnum; fl_size_ = size;
		Fl_FontSize* f = find(fnum, size);
		if (f != fl_fontsize) {
			fl_fontsize = f;
			fl_xfont = f->font;
			font_gc = 0;
		}
	}

	int fl_height() {
		return (fl_xfont->ascent + fl_xfont->descent);
	}

	int fl_descent() {
		return fl_xfont->descent;
	}

	double fl_width(const char* c, int n) {
		XCharStruct* p = fl_xfont->per_char;
		if (!p) return n*fl_xfont->min_bounds.width;
		int a = fl_xfont->min_char_or_byte2;
		int b = fl_xfont->max_char_or_byte2 - a;
		int w = 0;
		while (n--) {
			int x = *(uchar*)c++ - a;
			if (x >= 0 && x <= b) w += p[x].width;
			else w += fl_xfont->min_bounds.width;
		}
		return w;
	}

	double fl_width(uchar c) {
		XCharStruct* p = fl_xfont->per_char;
		if (p) {
			int a = fl_xfont->min_char_or_byte2;
			int b = fl_xfont->max_char_or_byte2 - a;
			int x = c-a;
			if (x >= 0 && x <= b) return p[x].width;
		}
		return fl_xfont->min_bounds.width;
	}

	//void fl_draw(const char* str, int n, int x, int y) {
	// if (font_gc != fl_gc) {
	//  if (!fl_xfont) fl_font(FL_HELVETICA, 14);
	//  font_gc = fl_gc;
	//  XSetFont(fl_display, fl_gc, fl_xfont->fid);
	//  }
	//char* strupdate;
	//strupdate="anita";
	//XDrawString(fl_display, fl_window, fl_gc, x, y, strupdate, strlen(strupdate));
	//}

	/*
		ccz_2009_02_10
		新增参数：
			font_wb：字体宽占多少个字节
			font_h：字模高
	*/
	void displaystr(int x, int y, int font_wb, int font_h, unsigned  *dispstr ,Display *play, Window  wind, GC gcc  ){
        unsigned r ;
        int xpoint, ypoint;
		xpoint=x;
		if(font_h==14){
			ypoint=y-13;		//高14小字体显示字模y轴偏移
		}else if(font_h==16){
			ypoint=y-15;		//高16中字体显示字模y轴偏移
		}else if(font_h==18){
			ypoint=y-17;		//高18中字体显示字模y轴偏移
		}else if(font_h==24){
			ypoint=y-23;		//高24大字体显示字模y轴偏移
		}else if(font_h==28){
			ypoint=y-27;		//高30(28)大字体显示字模y轴偏移
		}
		//逐行绘制点阵字模
		for (int row=0 ;row<font_h; row++) {
			for (int xiabiao=0 ;xiabiao<font_wb ;xiabiao ++){
				for (int w=0;w<8;w++){		//给第 row行，第 font_wb个字节
					r=  ((unsigned)(*(dispstr+row*font_wb+xiabiao)))<<w&255 ;
					if  (r>=128){
						XDrawPoint(play,wind,gcc,xpoint++,ypoint);//打点
					}
					else{
						xpoint++;
					}
				}
			}
			xpoint=x;
			ypoint++;
		}
	}
        void displaystr_Chinese(int x, int y, int font_wb, int font_h, unsigned char *dispstr ,Display *play, Window  wind, GC gcc  ){
        unsigned r ;
        int xpoint, ypoint;
		xpoint=x;
		if(font_h==14){
			ypoint=y-13;		//高14小字体显示字模y轴偏移
		}else if(font_h==16){
			ypoint=y-15;		//高16中字体显示字模y轴偏移
		}else if(font_h==18){
			ypoint=y-17;		//高18中字体显示字模y轴偏移
		}else if(font_h==24){
			ypoint=y-23;		//高24大字体显示字模y轴偏移
		}else if(font_h==28){
			ypoint=y-27;		//高30(28)大字体显示字模y轴偏移
		}
		//逐行绘制点阵字模
		for (int row=0 ;row<font_h; row++) {
			for (int xiabiao=0 ;xiabiao<font_wb ;xiabiao ++){
				for (int w=0;w<8;w++){		//给第 row行，第 font_wb个字节
					r=  ((unsigned char)(*(dispstr+row*font_wb+xiabiao)))<<w&255 ;
					if  (r>=128){
						XDrawPoint(play,wind,gcc,xpoint++,ypoint);//打点
					}
					else{
						xpoint++;
					}
				}
			}
			xpoint=x;
			ypoint++;
		}
	}

	/*  显示 ACSII字符 */
	int DrawOneEng(char *s,int x,int y,Display *play, Window  wind, GC gcc ){
		unsigned char qh;
		unsigned long offset;
		qh=*(s);
		offset=qh*16L;
		char buffer[16];

		memcpy(buffer,gw_Eng_Buf_2+offset,16);

		for(int i=0;i<16;i++){
			for(int k=0;k<8;k++){
				if(((buffer[i]>>(7-k))&0x1)!=0){
					//		fl_point(x+k,y+i);
					XDrawPoint(play,wind,gcc,x+k,y+i);
				}
			}
		}
		return 1;
	}

	//绘制越南字符，utf-8 到越南自定义字库影射！
	//?ÁáặẫạậảắấẩòớốổờộợỗưủúùữừếêểĐđíịỷọàệâÂơôỡóởõãềýƯầăựứồỏéẻĩỉ
	//?          00
	//Ááặẫạậảắấẩ    01-10
	//òớốổờộợỗưủ    11-20
	//úùữừếêểĐđí    21-30
	//ịỷọàệâÂơôỡ     31-40
	//óởõãềýƯầăự   41-50
	//ứồỏéẻĩỉ           51-57

	int DrawOneVerinan_Special(unsigned long *s,int x,int y,Display *play, Window  wind, GC gcc)//特殊字符的处理
	{
		if(fl_font_style != 0)//该特殊处理函数，只针对小字体，其他字体不处理
			return 0;

		unsigned qh=0;

		//yxy
		switch(*s)
		{
	        case 'ı'   :        qh=1;      break;   //ı
	        case 'ł'   :        qh=2;      break;
	        case 'ì'   :        qh=3;      break;
	        case 'î'   :        qh=4;      break;
	        case 'í'   :        qh=5;      break;
	        case '¡'   :        qh=6;      break;
	        case 'į'   :        qh=7;      break;
	        case 'ī'   :        qh=8;      break;
	        case 'ļ'   :        qh=9;      break;
	        case 'Í'   :        qh=10;     break;
	        case 'İ'   :        qh=11;     break;
	        case 'Į'   :        qh=12;     break;
	        case 'Ļ'   :        qh=13;     break;
		}

		if ( 0==qh ) {
		     return 0;
		}

		displaystr(x, y, gw_Verinan_Specill[qh].bt[2], 14, &gw_Verinan_Specill[qh].bt[CODEINFO_LEN], play, wind, gcc);
			return (int)(gw_Verinan_Specill[qh].bt[0]+WORDS_GAP);	//因为字符之间无空隙，所以要空出WORDS_GAP个像素

	}
	int DrawOneVerinan(unsigned long *s,int x,int y,Display *play, Window  wind, GC gcc ){
		unsigned qh;
		unsigned long offset;
		unsigned bf[48];//ccz_07_12_11
		//用到的就几个，以后可以扩充bht_12_07_30
		if(fl_font_style == 1){
			switch(*(s)){
			case 0xc582:		qh=1;break;//'ł'
			case 'è':		qh=2;break;//'è'
			case 'ê':		qh=3;break;//ê
			case 'ı':		qh=4;break;//ı
			case 'ş':		qh=5;break;//ş
			case 'é':		qh=6;break;//é
			case 'í':		qh=7;break;
			case 'ñ':		qh=8;break;
			case 'Ü':		qh=9;break;
			case 'ă':		qh=10;break;
			case 'Ž':		qh=11;break;
			case 'á':		qh=12;break;
			case 'ř':		qh=13;break;
			case 'ó':		qh=14;break;
			case 'ż':		qh=15;break;
			case 'ğ':		qh=16;break;
			case 'ö':		qh=17;break;
			case 'ü':		qh=18;break;
			case 'Ç':		qh=19;break;
			case '’':		qh=20;break;
			case 'Î':		qh=21;break;
			case 'ţ':		qh=22;break;
			case 'ý':		qh=23;break;
			case 'š':		qh=24;break;
			case 'ě':		qh=25;break;
			case 'ů':		qh=26;break;
			case 'ž':		qh=27;break;
			case 'ä':		qh=28;break;
			case 'đ':		qh=29;break;
			case 'Á':		qh=30;break;
			case 'ŕ':		qh=31;break;
			case 'ď':		qh=32;break;
			case 'Ď':		qh=33;break;
			case 'ξ':          	qh=34;         break;
			case 'ω':          	qh=35;         break;
			case 'π':          	qh=36;         break;
			case 'α':          	qh=37;         break;
			case 'δ':          	qh=38;         break;


			case 'Α' :          qh=39;         break;
			case 'ν' :          qh=40;         break;
			case 'ί' :          qh=41;         break;
			case 'Ε' :          qh=42;         break;
			case 'κ' :          qh=43;         break;
			case 'χ' :          qh=44;         break;
            case 'ú' :          qh=45;         break;
            case 'É' :          qh=46;         break;
            case 'Õ' :          qh=47;         break;
            case 'ő' :          qh=48;         break;
            case 'ī' :          qh=49;         break;
            case 'ņ' :          qh=50;         break;
            case 'ā' :          qh=51;         break;
            case 'ē' :          qh=52;         break;
            case 'ū' :          qh=53;         break;
            case 'ļ' :          qh=54;         break;
            case 'ķ' :          qh=55;         break;
            case 'Ģ' :          qh=56;         break;
            case 'Ā' :          qh=57;         break;
            case 'Ē' :          qh=58;         break;
            case 'ģ' :          qh=59;         break;
            case 'ô' :          qh=60;         break;
            case 'ľ' :          qh=61;         break;
            case 'Ľ' :          qh=62;         break;
            case 'ų' :          qh=63;         break;
            case 'į' :          qh=64;         break;
            case 'Į' :          qh=65;         break;
            case 'Ė' :          qh=66;         break;
            case 'ė' :          qh=67;         break;
            case 'Í' :          qh=68;         break;
            case 'č' :          qh=69;         break;
            case 'Ä' :          qh=70;         break;
            case 'Ų' :          qh=71;         break;
            case 'õ' :          qh=72;         break;
            case 'ą' :          qh=73;         break;
            case 'Š' :          qh=74;         break;
            case 'Ļ' :          qh=75;         break;

// 			case 'η' :          qh=39;         break;
// 			case 'ή' :          qh=40;         break;
// 			case 'ς' :          qh=41;         break;
// 			case 'β' :          qh=42;         break;
// 			case 'λ' :          qh=43;         break;
// 			case 'γ' :          qh=44;         break;
// 			case 'τ' :          qh=45;         break;
// 			case 'χ' :          qh=46;         break;
// 			case 'θ' :          qh=47;         break;
// 			case 'ε' :          qh=48;         break;
			default:
				qh=0;
				break;
			}
			displaystr(x,y, 3, 28, &gw_Verinan_28x18_Buf[qh].bt[CODEINFO_LEN] , fl_display , wind, gcc);
		return gw_Verinan_28x18_Buf[qh].bt[0];
		}
		switch(*(s)){
		case 0xc381:		qh=1;break;
		case 0xc3a1:		qh=2;break;
		case 0xe1bab7:	qh=3;break;
		case 0xe1baab:	qh=4;break;
		case 0xe1baa1:	qh=5;break;
		case 0xe1baad:	qh=6;break;
		case 0xe1baa3:	qh=7;break;
		case 0xe1baaf:	qh=8;break;
		case 0xe1baa5:	qh=9;break;
		case 0xe1baa9:	qh=10;break;
		case 0xc3b2:		qh=11;break;
		case 0xe1bb9b:	qh=12;break;
		case 0xe1bb91:	qh=13;break;
		case 0xe1bb95:	qh=14;break;
		case 0xe1bb9d:	qh=15;break;
		case 0xe1bb99:	qh=16;break;
		case 0xe1bba3:	qh=17;break;
		case 0xe1bb97:	qh=18;break;
		case 0xc6b0:		qh=19;break;
		case 0xe1bba7:	qh=20;break;
		case 0xc3ba:		qh=21;break;
		case 0xc3b9:		qh=22;break;
		case 0xe1bbaf:	qh=23;break;
		case 0xe1bbab:	qh=24;break;
		case 0xe1babf:	qh=25;break;
		case 0xc3aa:		qh=26;break;
		case 0xe1bb83:	qh=27;break;
		case 0xc490:		qh=28;break;
		case 0xc491:		qh=29;break;
		case 0xc3ad:		qh=30;break;
		case 0xe1bb8b:	qh=31;break;
		case 0xe1bbb7:	qh=32;break;
		case 0xe1bb8d:	qh=33;break;
		case 0xc3a0:		qh=34;break;
		case 0xe1bb87:	qh=35;break;
		case 0xc3a2:		qh=36;break;
		case 0xc382:		qh=37;break;
		case 0xc6a1:		qh=38;break;
		case 0xc3b4:		qh=39;break;
		case 0xe1bba1:	qh=40;break;
		case 0xc3b3:		qh=41;break;
		case 0xe1bb9f:	qh=42;break;
		case 0xc3b5:		qh=43;break;
		case 0xc3a3:		qh=44;break;
		case 0xe1bb81:	qh=45;break;
		case 0xc3bd:		qh=46;break;
		case 0xc6af:		qh=47;break;
		case 0xe1baa7:	qh=48;break;
		case 0xc483:		qh=49;break;
		case 0xe1bbb1:	qh=50;break;
		case 0xe1bba9:	qh=51;break;
		case 0xe1bb93:	qh=52;break;
		case 0xe1bb8f:	qh=53;break;
		case 0xc3a9:		qh=54;break;
		case 0xe1babb:	qh=55;break;
		case 0xc4a9:		qh=56;break;
		case 0xe1bb89:	qh=57;break;
		case 0xc3ac:		qh=58;break;
		case 0xe1bba5:	qh=59;break;
		case 0xe1babd:	qh=60;break;
		case 0xc591:		qh=61;break;
		case 0xc3b6:		qh=62;break;
		case 0xc3bc:		qh=63;break;
		case 0xc393:		qh=64;break;
		case 0xc39a:		qh=65;break;
		case 0xc389:		qh=66;break;
		//hqj_06_10_23 start
		case 0xc599:		qh=67;break;  //ck-206-10-17 ,CZ ,ř'
		case 0xc5af:		qh=68;break;  //ck-206-10-18 ,CZ ,
		case 0xc5be:		qh=69;break;  //ck-206-10-18 ,CZ ,
		case 0xc49b:		qh=70;break;  //ck-206-10-18 ,CZ ,
		case 0xc5a5:		qh=71;break;  //ck-206-10-18 ,CZ , ť c5a5
		case 0xc48f:		qh=72;break;  //ck-206-10-18 ,CZ ,
		case 0xc5bd:		qh=73;break;  //ck-206-10-18 ,CZ ,
		case 0xc48c:		qh=74;break;  //ck-206-10-18 ,CZ ,
		case 0xc48d:		qh=75;break;  //ck-206-10-18 ,CZ ,
		case 0xc5a1:		qh=76;break;  //ck-206-10-18 ,CZ ,
		case 0xc588:    	qh=77;break;  //ck-206-10-18 ,CZ ,
		case 0xc39d:    	qh=78;break;  //ck-206-10-18 ,CZ ,
		case 0xc598:    	qh=79;break;  //ck-206-10-18 ,CZ ,
		case 0xc38d:    	qh=80;break;  //ck-206-10-18 ,CZ ,
		case 0xc5ae:    	qh=81;break;  //ck-206-10-18 ,CZ ,
		case 0xc5a0:    	qh=82;break;  //ck-206-10-18 ,CZ ,
    //hqj_06_10_23 end
		case 0xc3a7 :		qh=83 ; break ; //ck-207-06-09 , pt , ç
//ccz_2009_02_06 start
		//波兰语18个特殊字符，新增16个
		case 0xC484:		qh=84;break;//Ą	//01
		case 0xC485:		qh=85;break;//ą
		case 0xC486:		qh=86;break;//Ć
		case 0xC487:		qh=87;break;//ć
		case 0xC498:		qh=88;break;//Ę
		case 0xC499:		qh=89;break;//ę
		case 0xC581:		qh=90;break;//Ł
		case 0xC582:		qh=91;break;//ł
		case 0xC583:		qh=92;break;//Ń
		case 0xC584:		qh=93;break;//ń		//10
		case 0xC59A:		qh=94;break;//Ś
		case 0xC59B:		qh=95;break;//ś
		case 0xC5B9:		qh=96;break;//Ź
		case 0xC5BA:		qh=97;break;//ź
		case 0xC5BB:		qh=98;break;//Ż
		case 0xC5BC:		qh=99;break;//ż		//16

		//土耳其语12个特殊字符，新增9个
		case 0xC387:		qh=100;break;//Ç		//01
		case 0xC49E:		qh=101;break;//Ğ
		case 0xC49F:		qh=102;break;//ğ
		case 0xC4B1:		qh=103;break;//ı
		case 0xC4B0:		qh=104;break;//İ
		case 0xC396:		qh=105;break;//Ö
		case 0xC59E:		qh=106;break;//Ş
		case 0xC59F:		qh=107;break;//ş
		case 0xC39C:		qh=108;break;//Ü		//09

		//法文新增13个特殊字符：èËëÎîÏïÔÛûÙŸÿ
		case 0xC3A8:		qh=109;		break;
		case 0xC38B:		qh=110;		break;
		case 0xC3AB:		qh=111;		break;
		case 0xC38E:		qh=112;		break;
		case 0xC3AE:		qh=113;		break;
		case 0xC38F:		qh=114;		break;
		case 0xC3AF:		qh=115;		break;
		case 0xC394:		qh=116;		break;
		case 0xC39B:		qh=117;		break;
		case 0xC3BB:		qh=118;		break;
		case 0xC399:		qh=119;		break;
		case 0xC5B8:		qh=120;		break;
		case 0xC3BF:		qh=121;		break;

		//新增省略号
		case 0xE280A6:	qh=122;		break;
		//新增特殊字符（在俄文中显示病历号出现的）№
		case 0xE28496:	qh=123;		break;
		//新增左上角小圆圈
		case 0xC2B0:		qh=124;		break;
		//新增西班牙文特殊字符ñÑº
		case 0xC3B1:		qh=125;		break;
		case 0xC391:		qh=126;		break;
		case 0xC2BA:		qh=127;		break;
//新增特殊文字 	//äţæøÅåŐµűØĂÆ//
		case 0xc3a4 :		qh=128;		break;//ä//
		case 0xc5a3 :		qh=129;		break;//ţ//
		case 0xc3a6 :		qh=130;		break;//æ//
		case 0xc3b8 :		qh=131;		break;//ø//
		case 0xc385 :		qh=132;		break;//Å//
		case 0xc3a5 :		qh=133;		break;//å//
		case 0xc590 :		qh=134;		break;//Ő//
		case 0xc2b5 :		qh=135;		break;//µ//
		case 0xc5b1 :		qh=136;		break;//ű
		case 0xc398 :		qh=137;		break;//Ø//
		case 0xc482 :		qh=138;		break;//Ă//
		case 0xc386 :		qh=139;		break;//Æ//
		case 0xe2809e :		qh=140;		break;//„
		case 0xc384 :		qh=141;		break;// Ä  
		case 0xc39f :		qh=142;		break;//ß
                case 0xc2a1 :           qh=143;         break;//¡
                case 0xc2bf :           qh=144;         break;//¿
                case 0xc2a0 :           qh=145;         break;//    空格
                case 0xe28099:          qh=146;         break;//’
                case 0xc48e :           qh=147;         break;//Ď
                case 0xc595 :          qh=148;         break;// ŕ

                case 'ξ' :          qh=149;         break;
                case 'ω' :          qh=150;         break;
                case 'π' :          qh=151;         break;
                case 'α' :          qh=152;         break;
                case 'δ' :          qh=153;         break;
                case 'η' :          qh=154;         break;
                case 'ή' :          qh=155;         break;
                case 'ς' :          qh=156;         break;
              	case 'β' :          qh=157;         break;
                case 'λ' :          qh=158;         break;
                case 'γ' :          qh=159;         break;
                case 'τ' :          qh=160;         break;
                case 'χ' :          qh=161;         break;
                case 'θ' :          qh=162;         break;
             	case 'ε' :          qh=163;         break;

                case 'ρ' :          qh=164;         break;
                case 'ό' :          qh=165;         break;
                case 'ύ' :          qh=166;         break;
                case 'σ' :          qh=167;         break;
                case 'έ' :          qh=168;         break;
             	case 'κ' :          qh=169;         break;
                case 'ι' :          qh=170;         break;
                case 'Σ' :          qh=171;         break;
             	case 'Π' :          qh=172;         break;
                case 'μ' :          qh=173;         break;
             	case 'ψ' :          qh=174;         break;
                case 'Δ' :          qh=175;         break;
             	case 'Θ' :          qh=176;         break;
                case 'Ξ' :          qh=177;         break;
                case 'Õ' :          qh=178;         break;
                case 'ī' :          qh=179;         break;
                case 'ņ' :          qh=180;         break;
                case 'ā' :          qh=181;         break;
                case 'ē' :          qh=182;         break;
                case 'ū' :          qh=183;         break;
                case 'ļ' :          qh=184;         break;
                case 'ķ' :          qh=185;         break;
                case 'Ģ' :          qh=186;         break;
                case 'Ā' :          qh=187;         break;
                case 'Ē' :          qh=188;         break;
                case 'ģ' :          qh=189;         break;
                case 'ľ' :          qh=190;         break;
                case 'Ľ' :          qh=191;         break;
                case 'ų' :          qh=192;         break;
                case 'į' :          qh=193;         break;
                case 'Į' :          qh=194;         break;
                case 'Ė' :          qh=195;         break;
                case 'ė' :          qh=196;         break;
                case 'Ų' :          qh=197;         break;
                case 'Ū' :          qh=198;         break;
                case '”' :          qh=199;         break;
                case '“' :          qh=200;         break;


                //case 'ô' :  break;

                //case 'ť': break;



                //case 'Ü' :          qh=177;         break;
                //case 'ú' :          qh=178;         break;

//              	case 'ε' :          qh=177;         break;
//                 case 'θ' :          qh=178;         break;
//              	case 'ε' :          qh=179;         break;
		default:
			qh=0;
			//printf ("DrawOneVerinan *(s)=%x , qh=%d \n" , *(s) , qh) ;
			break;
		}
		if(fl_font_style == 0){		//ccz_07_12_07 start
			offset=qh*16L;
			char buffer[16];
			memcpy(buffer,gw_Verinan_Buf+offset,16);
			for(int i=0;i<16;i++){
				for(int k=0;k<8;k++){
					if(((buffer[i]>>(7-k))&0x1)!=0){
						XDrawPoint(play,wind,gcc,x+k,y+i);
					}
				}
			}
		}else{
			displaystr(x,y, 2, 24, &gw_Verinan_24x16_Buf[qh].bt[0] , fl_display , wind, gcc);
		}				//ccz_07_12_07 end
		return 1;
	}

//ck-207-09-29 start
	int DrawOne14(unsigned long *s,int x,int y,Display *play, Window  wind, GC gcc ) { //ck-207-08-14
		unsigned qh,qh1;
       		qh=JapenCodeTransform(*s) ;
		//printf ("JapenCodeTransform return %d \n" ,qh) ;
// 		if ( 0 == qh) {
// 		return 0 ;
// 		}
		if(qh){
			if(fl_font_style == 0){		//ccz_07_12_04 start
				displaystr(x, y, 2, 14, &Chars14_14[qh].bt[0], play, wind, gcc) ;
				return 16 ;
			}else{
				displaystr(x, y, 4, 24, &Chars24_24[qh].bt[0], play, wind, gcc) ;
				return 24 ;
			}				//ccz_07_12_04 end
		}
		else{
			qh1 = KoreaCodeTransform(*s);
			if(qh1){
				displaystr(x, y, 2, 16, &gw_Korea[qh1].bt[3], play, wind, gcc) ;
				return 15;
			}
			else{
				return 0;
			}
		}
	}
//ck-207-09-29 end

	int DrawRussia(unsigned long *s, int x, int y, Display *play, Window wind, GC gcc){
		unsigned qh;
		unsigned bf[100];

		qh= RussiaCodeTransform(*(s)) ;
		if ( 0==qh ) {
		return 0;
		}
		if(fl_font_style == 0){
			displaystr(x, y, gw_Russia[qh].bt[2], 14, &gw_Russia[qh].bt[CODEINFO_LEN], play, wind, gcc);
			return (int)(gw_Russia[qh].bt[0]+WORDS_GAP);	//因为字符之间无空隙，所以要空出WORDS_GAP个像素
		}else{
			displaystr(x, y, gw_Russia2[qh].bt[2], 24, &gw_Russia2[qh].bt[CODEINFO_LEN], play, wind, gcc);
			return (int)(gw_Russia2[qh].bt[0]+WORDS_GAP);	//因为字符之间无空隙，所以要空出WORDS_GAP个像素
		}
	}
   

	void fl_draw(const char* str, int n, int x, int y) {
		unsigned   dispfont[200]={0};
		long       fff;
		int        serrior=0;
		int        forvar;
		unsigned long  welldisplay[1000];
		unsigned char display;
		int        xb=0;
		y=y+1;
		bool disp=false;
		int ret=0;
                unsigned long  unicode_Value;
                char  gb2312Char[6];   //临时保存单一字符的Gb2312编码
                unsigned long gb2313Display[1000];  // 如果是中文，保存该字符的GB2312编码，如果不是中文，保存该字符的UTF8编码  
                unsigned long  offset =0;

                Set_HZK14_Matrice(HZK14_buffer);              

		if (font_gc != fl_gc) {
			if (!fl_xfont) fl_font(FL_HELVETICA, 14);
			font_gc = fl_gc;
			XSetFont(fl_display, fl_gc, fl_xfont->fid);
		}
		while (*str!=0x00){    //原始字符串结束符判定！
			display=*str;
			if   (display>=0x80 && display<0xe0){                       //双字节编码字符，可为汉字或双字节越南字符！
				disp=true;
				welldisplay[xb]= display ;
				str++;
				display=*str;
				welldisplay[xb]=(welldisplay[xb])*256+display;
                                gb2313Display[xb] = welldisplay[xb];
				xb++;
				str++;

			}
			else{
				if(display>0xe0 ){ 					//三字节编码越南字符！ 
                                        // 判断是否为中文  
                                                        
                                        unicode_Value = Utf8_to_Unicode(str);                                                                                  
                                        if( Is_Chinese_Word(unicode_Value) == 1)           // 如果是中文
                                        {
                                            disp = true;
                                            welldisplay[xb] = unicode_Value;                              
                                            
                                            Utf8_To_GB2312((char*)str,3,gb2312Char,6); // Ｕtf8字符转为gb2312编码
                                                                                   
                                            gb2313Display[xb] =((unsigned char)gb2312Char[0])*256 + (unsigned char)gb2312Char[1];
                                                                                  
                                            str+=3;
                                            xb++;
                                        }
                                        else          // 三字节编码越南字符！
                                        {
                                           disp=true;
					   welldisplay[xb]= display*256*256;
					   str++;
					   display=*str;
					   welldisplay[xb]=(welldisplay[xb])+display*256;
					   str++;
					   display=*str;
					   welldisplay[xb]=(welldisplay[xb])+display; 
                                           gb2313Display[xb] = welldisplay[xb];                       
                                           
					   str++;
					   xb++;
                                        }					
				}
				else{                          //标准ASCII字符！
					welldisplay[xb]=*str;
                                        gb2313Display[xb] = welldisplay[xb];
					str++;
					xb++;
				}
			}
		}

		welldisplay[xb]=0;		//字符串编码数组，最后一个值赋零！

		while (welldisplay[serrior]!=0){//当为零时，该字符串处理完毕！
                        if((welldisplay[serrior] >= 0x4E00 && welldisplay[serrior] <= 0x9FA5))
                           //||(welldisplay[serrior] >= 0x2E80 && welldisplay[serrior] <= 0xA4CF))                          
                        {
                              // 中文汉字显示  
                             offset = ((gb2313Display[serrior]/256 -0xa1)*94 + (gb2313Display[serrior]% 256 -0xa1))*28;                                             
                             if(offset >= HZK14_BUFFER_LEN) //预防地址越界,程序崩溃
                             {    
                                  printf("Display Chinese word error!\n");                             
                                  offset = 0;
                             }
                             displaystr_Chinese(x, y, 2, 14, &HZK14_buffer[offset],fl_display, fl_window, fl_gc);           //modify by zxs on 2014/01/16
                             x=x+16;
                        }
/*
			if (welldisplay[serrior]>0x8000 && welldisplay[serrior]<0xc000){    //中文汉字编码！
				fff=welldisplay[serrior]-1-32768;
				if(fl_font_style == 0){                 //ccz_07_05_12 start
					//displaystr(x, y, 2, 14, &Chars14_CHINESE[fff].bt[0] , fl_display, fl_window, fl_gc);
                                         displaystr(x, y, 2, 14, &HZK14_buffer[fff].bt[0] , fl_display, fl_window, fl_gc);            //modify by zxs on 2014/01/16

					x=x+16;
				}
#ifdef  VITAS
				else if (fl_font_style == 2) {
					displaystr(x, y, 2, 18, &China18[fff].bt[CODEINFO_LEN] , fl_display, fl_window, fl_gc);   //                    printf ("Ascii18[fff].bt[0]=%d\n" ,Ascii18[fff].bt[0]) ;
					x=x+China18[fff].bt[0];
				} else if (fl_font_style == 1) {
					displaystr(x, y, 4, 28, &China28[fff].bt[CODEINFO_LEN] , fl_display, fl_window, fl_gc);
					x=x+China28[fff].bt[0];
				}
#else
                		else{
					displaystr(x, y, 4, 24, &Chars24_CHINESE[fff].bt[0], fl_display, fl_window, fl_gc);
					x=x+24;
				}
#endif
//ccz_07_05_12 end
			}*/

			if (welldisplay[serrior]>0xc000 ){          //越南文编码可以是两字节或三字节！                              
				if ( DrawOne14 ( &welldisplay[serrior] , x , y , fl_display , fl_window , fl_gc )==0 ) { //当DRAWONE14没有找到字的情况下才做DrawOneVerinan //如果全角字符打印中找不到，不是全角字符    //ck-207-09-28 start
					ret = DrawRussia(&welldisplay[serrior],x,y,fl_display,fl_window,fl_gc);
					if(ret == 0)//俄文中 ，找不到，到特殊字库中寻找
						ret = DrawOneVerinan_Special(&welldisplay[serrior],x,y,fl_display,fl_window,fl_gc);


					if(ret == 0){	//俄文字库中找不到  或特殊处理字库中，也找不到
						if(fl_font_style == 0){
							DrawOneVerinan(&welldisplay[serrior], x, y-14, fl_display, fl_window, fl_gc);
							x=x+8;
						}
						else if(fl_font_style == 1){
							ret = DrawOneVerinan(&welldisplay[serrior],x,y,fl_display,fl_window,fl_gc );
							x=x+ret;
						}else{
							DrawOneVerinan(&welldisplay[serrior],x,y,fl_display,fl_window,fl_gc );
							x=x+10;
						}
					}else{
						x=x+ret;
					}
				} else {  //是全角字符的话
					if(fl_font_style == 0){		//ccz_07_12_06 start
						x=x+16;
					}else{
						x=x+24;
					}				//ccz_07_12_06 end
				}  //ck-207-09-28 end
			}   
                    
			if(welldisplay[serrior]<0xff){ //英语。。。？
				char bbb=0x96;
#ifdef VITAS
				if(fl_font_style == 0){
					bbb=welldisplay[serrior];                       //标准ASCII码！
					if (bbb>'~'||bbb<' ') {
						DrawOneEng(&bbb,x,y-13,fl_display,fl_window,fl_gc );
						x=x+8;
					} else {
						displaystr(x, y, AsciiPhilips16[bbb].bt[2], 16, &AsciiPhilips16[bbb].bt[CODEINFO_LEN], fl_display, fl_window, fl_gc);
						x=x+AsciiPhilips16[bbb].bt[0];
					}
				}else if(fl_font_style == 2) {
					fff=welldisplay[serrior];
					if (fff>'~'||fff<' ') {
						printf ("fl_font_style == 2 not ready") ;
					} else {
						displaystr(x, y, Ascii18[fff].bt[2], 18, &Ascii18[fff].bt[CODEINFO_LEN], fl_display, fl_window, fl_gc);
						x=x+Ascii18[fff].bt[0];
					}
				}else if(fl_font_style == 1) {
					fff=welldisplay[serrior];//Ascii28
					if (fff>'~'||fff<' ') {
						printf ("fl_font_style == 1 not ready") ;
					} else {
						displaystr(x, y, Ascii28[fff].bt[2], 28, &Ascii28[fff].bt[CODEINFO_LEN], fl_display, fl_window, fl_gc);
						x=x+Ascii28[fff].bt[0];
					}
				}
#else
				if(fl_font_style == 0){					//ccz_07_05_14 start
					bbb=welldisplay[serrior];                       //标准ASCII码！
					DrawOneEng(&bbb,x,y-13,fl_display,fl_window,fl_gc );
					x=x+8;
				}else{
					fff=welldisplay[serrior];
					displaystr(x, y, 2, 24,&Ascii24[fff].bt[0], fl_display, fl_window, fl_gc);
					x=x+14;
				}							//ccz_07_05_14 end
#endif
			}
			serrior++;
		}   //while end
	}
    unsigned * ExtractStrDiplay( unsigned* dispfont , unsigned* string , int len ) {
        int forvar ;
//         for(forvar=0;forvar<len;forvar++){
//             *(dispfont+forvar)=*(string+forvar+3);
//         }
        memcpy(dispfont,string,len*sizeof(unsigned)) ;
        return dispfont ;
    }
	// int fl_TXT_WEIGHT(const char* name, int TxtTyle) {
	// printf ("abc\n") ;
	// }
    unsigned RussiaCodeTransform(unsigned long testcode){//retrun gw_Russia and gw_Russia2 index
        switch(testcode){
            case 0xD090:        return 1;       break;  //А
            case 0xD091:        return 2;       break;  //Б
            case 0xD092:        return 3;       break;  //В
            case 0xD093:        return 4;       break;  //Г
            case 0xD094:        return 5;       break;  //Д
            case 0xD095:        return 6;       break;  //Е
            case 0xD081:        return 7;       break;  //Ё
            case 0xD096:        return 8;       break;  //Ж
            case 0xD097:        return 9;       break;  //З
            case 0xD098:        return 10;      break;  //И
            case 0xD099:        return 11;      break;  //Й
            case 0xD09A:        return 12;      break;  //К
            case 0xD09B:        return 13;      break;  //Л
            case 0xD09C:        return 14;      break;  //М
            case 0xD09D:        return 15;      break;  //Н
            case 0xD09E:        return 16;      break;  //О
            case 0xD09F:        return 17;      break;  //П
            case 0xD0A0:        return 18;      break;  //Р
            case 0xD0A1:        return 19;      break;  //С
            case 0xD0A2:        return 20;      break;  //Т
            case 0xD0A3:        return 21;      break;  //У
            case 0xD0A4:        return 22;      break;  //Ф
            case 0xD0A5:        return 23;      break;  //Х
            case 0xD0A6:        return 24;      break;  //Ц
            case 0xD0A7:        return 25;      break;  //Ч
            case 0xD0A8:        return 26;      break;  //Ш
            case 0xD0A9:        return 27;      break;  //Щ
            case 0xD0AB:        return 28;      break;  //Ы
            case 0xD0AD:        return 29;      break;  //Э
            case 0xD0AE:        return 30;      break;  //Ю
            case 0xD0AF:        return 31;      break;  //Я
            case 0xD0B0:        return 32;      break;  //а
            case 0xD0B1:        return 33;      break;  //б
            case 0xD0B2:        return 34;      break;  //в
            case 0xD0B3:        return 35;      break;  //г
            case 0xD0B4:        return 36;      break;  //д
            case 0xD0B5:        return 37;      break;  //е
            case 0xD191:        return 38;      break;  //ё
            case 0xD0B6:        return 39;      break;  //ж
            case 0xD0B7:        return 40;      break;  //з
            case 0xD0B8:        return 41;      break;  //и
            case 0xD0B9:        return 42;      break;  //й
            case 0xD0BA:        return 43;      break;  //к
            case 0xD0BB:        return 44;      break;  //л
            case 0xD0BC:        return 45;      break;  //м
            case 0xD0BD:        return 46;      break;  //н
            case 0xD0BE:        return 47;      break;  //о
            case 0xD0BF:        return 48;      break;  //п
            case 0xD180:        return 49;      break;  //р
            case 0xD181:        return 50;      break;  //с
            case 0xD182:        return 51;      break;  //т
            case 0xD183:        return 52;      break;  //у
            case 0xD184:        return 53;      break;  //ф
            case 0xD185:        return 54;      break;  //х
            case 0xD186:        return 55;      break;  //ц
            case 0xD187:        return 56;      break;  //ч
            case 0xD188:        return 57;      break;  //ш
            case 0xD189:        return 58;      break;  //щ
            case 0xD18A:        return 59;      break;  //ъ
            case 0xD18B:        return 60;      break;  //ы
            case 0xD18C:        return 61;      break;  //ь
            case 0xD18D:        return 62;      break;  //э
            case 0xD18E:        return 63;      break;  //ю
            case 0xD18F:        return 64;      break;  //я
            case 0xD0AC:        return 65;      break;  //Ь
            case 0xC380:        return 66;      break;  //À//新增意大利语特殊字符À
            case 0xC383:        return 67;      break;  //Ã//新增葡萄牙语特殊字符Ã
            case 0xe28692:      return 68;      break;  //→
            case 0xe28690:      return 69;      break;  //←
            case 0xc5a2 :       return 70;      break;  //Ţ
            case 0xe28093:      return 71;      break;  //–
	        case 0xC388:	    return 72;      break;  //È
            default:
               // printf ("Cant find Russia *(s)=%x  \n" , testcode ) ;
                return 0;
                break;
        }
    }//end unsigned RussiaCodeTransform(long testcode){
    unsigned JapenCodeTransform(unsigned long testcode){//return  Chars14_14 and Chars24_24 index
        switch(testcode){
        case 0xe383a1 :     return 1 ;      break ; //メ
        case 0xe382a4 :     return 2 ;      break ; //イ
        case 0xe383b3 :     return 3 ;      break ; //ン
        case 0xe3838b :     return 4 ;      break ; //ニ
        case 0xe383a5 :     return 5 ;      break ; //ュ
        case 0xe383bc :     return 6 ;      break ; //ー
        case 0xe98080 :     return 7 ;      break ; //
        case 0xe587ba :     return 8 ;      break ; //
        case 0xe7b582 :     return 9 ;      break ; //終
        case 0xe4ba86 :     return 10 ;     break ; //了
        case 0xe8a8ad :     return 11 ;     break ; //設
        case 0xe5ae9a :     return 12 ;     break ; //定
        case 0xe383aa :     return 13 ;     break ; //リ
        case 0xe38389 :     return 14 ;     break ; //ド
        case 0xe5b08e :     return 15 ;     break ; //導
        case 0xe7b79a :     return 16 ;     break ; //線
        case 0xe588a9 :     return 17 ;     break ; //利
        case 0xe5be97 :     return 18 ;     break ; //得
        case 0xe382ab :     return 19 ;     break ; //カ
        case 0xe383ad :     return 20 ;     break ; //ロ
        case 0xe3839a :     return 21 ;     break ; //ペ
        case 0xe382b9 :     return 22 ;     break ; //ス
        case 0xe6a49c :     return 23 ;     break ; //検
        case 0xe6b3a2 :     return 24 ;     break ; //波
        case 0xe4b88a :     return 25 ;     break ; //上
        case 0xe99990 :     return 26 ;     break ; //限
        case 0xe4b88b :     return 27 ;     break ; //下
        case 0xe382a2 :     return 28 ;     break ; //ア
        case 0xe383a9 :     return 29 ;     break ; //ラ
        case 0xe383a0 :     return 30 ;     break ; //ム
        case 0xe58cba :     return 31 ;     break ; //区
        case 0xe58886 :     return 32 ;     break ; //分
        case 0xe38399 :     return 33 ;     break ; //ベ
        case 0xe38383 :     return 34 ;     break ; //ッ
        case 0xe383a2 :     return 35 ;     break ; //モ
        case 0xe382aa :     return 36 ;     break ; //オ
        case 0xe38395 :     return 37 ;     break ; //フ
        case 0xe4b8ad :     return 38 ;     break ; //中
        case 0xe5a4ae :     return 39 ;     break ; //央
        case 0xe983a8 :     return 40 ;     break ; //部
        case 0xe6a99f :     return 41 ;     break ; //機
        case 0xe996a2 :     return 42 ;     break ; //関
        case 0xe5bda2 :     return 43 ;     break ; //形
        case 0xe9809f :     return 44 ;     break ; //速
        case 0xe5baa6 :     return 45 ;     break ; //度
        case 0xe3838d :     return 46 ;     break ; //ネ
        case 0xe38388 :     return 47 ;     break ; //ト
        case 0xe383af :     return 48 ;     break ; //ワ
        case 0xe382af :     return 49 ;     break ; //ク
        case 0xe3839d :     return 50 ;     break ; //ポ
        case 0xe382b7 :     return 51 ;     break ; //シ
        case 0xe383ab :     return 52 ;     break ; //ル
        case 0xe4bfa1 :     return 53 ;     break ; //信
        case 0xe58fb7 :     return 54 ;     break ; //号
        case 0xe38391 :     return 55 ;     break ; //パ
        case 0xe3818c :     return 56 ;     break ; //が
        case 0xe99693 :     return 57 ;     break ; //間
        case 0xe98195 :     return 58 ;     break ; //違
        case 0xe381a3 :     return 59 ;     break ; //っ
        case 0xe381a6 :     return 60 ;     break ; //て
        case 0xe38184 :     return 61 ;     break ; //い
        case 0xe381be :     return 62 ;     break ; //ま
        case 0xe38199 :     return 63 ;     break ; //す
        case 0xe38282 :     return 64 ;     break ; //も
        case 0xe38186 :     return 65 ;     break ; //う
        case 0xe4b880 :     return 66 ;     break ; //一
        case 0xe585a5 :     return 67 ;     break ; //入
        case 0xe58a9b :     return 68 ;     break ; //力
        case 0xe38197 :     return 69 ;     break ; //し
        case 0xe38195 :     return 70 ;     break ; //さ
        case 0xe58fa4 :     return 71 ;     break ; //古
        case 0xe3818f :     return 72 ;     break ; //く
        case 0xe381a0 :     return 73 ;     break ; //だ
        case 0xe696b0 :     return 74 ;     break ; //新
        case 0xe381ae :     return 75 ;     break ; //の
        case 0xe7a2ba :     return 76 ;     break ; //確
        case 0xe8aa8d :     return 77 ;     break ; //認
        case 0xe381aa :     return 78 ;     break ; //な
        case 0xe4bdbf :     return 79 ;     break ; //使
        case 0xe794a8 :     return 80 ;     break ; //用
        case 0xe88085 :     return 81 ;     break ; //者
        case 0xe38292 :     return 82 ;     break ; //を
        case 0xe981b8 :     return 83 ;     break ; //選
        case 0xe68a9e :     return 84 ;     break ; //択
        case 0xe381a7 :     return 85 ;     break ; //で
        case 0xe3818d :     return 86 ;     break ; //き
        case 0xe381af :     return 87 ;     break ; //は
        case 0xe38188 :     return 88 ;     break ; //え
        case 0xe38387 :     return 89 ;     break ; //デ
        case 0xe382bf :     return 90 ;     break ; //タ
        case 0xe68e83 :     return 91 ;     break ; //掃
        case 0xe3818b :     return 92 ;     break ; //か
        case 0xe3819b :     return 93 ;     break ; //せ
        case 0xe38293 :     return 94 ;     break ; //ん
        case 0xe682a3 :     return 95 ;     break ; //患
        case 0xe69982 :     return 96 ;     break ; //時
        case 0xe382a3 :     return 97 ;     break ; //ィ
        case 0xe38397 :     return 98 ;     break ; //プ
        case 0xe383ac :     return 99 ;     break ; //レ
        case 0xe4bd9c :     return 100 ;        break ; //作
        case 0xe6a5ad :     return 101 ;        break ; //業
        case 0xe68f9b :     return 102 ;        break ; //換
        case 0xe7ae97 :     return 103 ;        break ; //算
        case 0xe6b097 :     return 104 ;        break ; //気
        case 0xe4bd93 :     return 105 ;        break ; //体
        case 0xe8a39c :     return 106 ;        break ; //補
        case 0xe6ada3 :     return 107 ;        break ; //正
        case 0xe985b8 :     return 108 ;        break ; //酸
        case 0xe7b4a0 :     return 109 ;        break ; //素
        case 0xe58c96 :     return 110 ;        break ; //化
        case 0xe4ba8c :     return 111 ;        break ; //二
        case 0xe7aa92 :     return 112 ;        break ; //窒
        case 0xe3839e :     return 113 ;        break ; //マ
        case 0xe887aa :     return 114 ;        break ; //自
        case 0xe58b95 :     return 115 ;        break ; //動
        case 0xe382bc :     return 116 ;        break ; //ゼ
        case 0xe381ab :     return 117 ;        break ; //に
        case 0xe59088 :     return 118 ;        break ; //合
        case 0xe3828f :     return 119 ;        break ; //わ
        case 0xe3828b :     return 120 ;        break ; //る
        case 0xe5bcb7 :     return 121 ;        break ; //強
        case 0xe588b6 :     return 122 ;        break ; //制
        case 0xe79a84 :     return 123 ;        break ; //的
        case 0xe58d98 :     return 124 ;        break ; //単
        case 0xe4bd8d :     return 125 ;        break ; //位
        case 0xe8a18c :     return 126 ;        break ; //行
        case 0xe5a49c :     return 127 ;        break ; //夜
        case 0xe8aabf :     return 128 ;        break ; //調
        case 0xe7af80 :     return 129 ;        break ; //節
        case 0xe58db0 :     return 130 ;        break ; //印
        case 0xe588b7 :     return 131 ;        break ; //刷
        case 0xe5898d :     return 132 ;        break ; //前
        case 0xe59b9e :     return 133 ;        break ; //回
        case 0xe58887 :     return 134 ;        break ; //切
        case 0xe7a792 :     return 135 ;        break ; //秒
        case 0xe68ba1 :     return 136 ;        break ; //拡
        case 0xe5bcb5 :     return 137 ;        break ; //張
        case 0xe6b8ac :     return 138 ;        break ; //測
        case 0xe38182 :     return 139 ;        break ; //あ
        case 0xe3828a :     return 140 ;        break ; //り
        case 0xe6ba90 :     return 141 ;        break ; //源
        case 0xe782ad :     return 142 ;        break ; //炭
        case 0xe38390 :     return 143 ;        break ; //バ
        case 0xe58585 :     return 144 ;        break ; //充
        case 0xe5a1ab :     return 145 ;        break ; //填
        case 0xe5a4a7 :     return 146 ;        break ; //大
        case 0xe4baba :     return 147 ;        break ; //人
        case 0xe5b08f :     return 148 ;        break ; //小
        case 0xe58590 :     return 149 ;        break ; //児
        case 0xe7949f :     return 150 ;        break ; //生
        case 0xe99a94 :     return 151 ;        break ; //隔
        case 0xe6b8a9 :     return 152 ;        break ; //温
        case 0xe69182 :     return 153 ;        break ; //摂
        case 0xe6b08f :     return 154 ;        break ; //氏
        case 0xe88faf :     return 155 ;        break ; //華
        case 0xe5868d :     return 156 ;        break ; //再
        case 0xe78fbe :     return 157 ;        break ; //現
        case 0xe8a1a8 :     return 158 ;        break ; //表
        case 0xe7a4ba :     return 159 ;        break ; //示
        case 0xe69e90 :     return 160 ;        break ; //析
        case 0xe38381 :     return 161 ;        break ; //チ
        case 0xe383a3 :     return 162 ;        break ; //ャ
        case 0xe68385 :     return 163 ;        break ; //情
        case 0xe5a0b1 :     return 164 ;        break ; //報
        case 0xe697a5 :     return 165 ;        break ; //日
        case 0xe4bb98 :     return 166 ;        break ; //付
        case 0xe58685 :     return 167 ;        break ; //内
        case 0xe5aeb9 :     return 168 ;        break ; //容
        case 0xe382ac :     return 169 ;        break ; //ガ
        case 0xe8aa98 :     return 170 ;        break ; //誘
        case 0xe59ba0 :     return 171 ;        break ; //因
        case 0xe995b7 :     return 172 ;        break ; //長
        case 0xe382a9 :     return 173 ;        break ; //ォ
        case 0xe383b4 :     return 174 ;        break ; //ヴ
        case 0xe381a8 :     return 175 ;        break ; //と
        case 0xe6988e :     return 176 ;        break ; //明
        case 0xe4bb96 :     return 177 ;        break ; //他
        case 0xe382b0 :     return 178 ;        break ; //グ
        case 0xe3838a :     return 179 ;        break ; //ナ
        case 0xe58f97 :     return 180 ;        break ; //受
        case 0xe3819f :     return 181 ;        break ; //た
        case 0xe5bf83 :     return 182 ;        break ; //心
        case 0xe5819c :     return 183 ;        break ; //停
        case 0xe6ada2 :     return 184 ;        break ; //止
        case 0xe68a9c :     return 185 ;        break ; //抜
        case 0xe38191 :     return 186 ;        break ; //け
        case 0xe68ea2 :     return 187 ;        break ; //探
        case 0xe9879d :     return 188 ;        break ; //針
        case 0xe782b9 :     return 189 ;        break ; //点
        case 0xe5a489 :     return 190 ;        break ; //変
        case 0xe599a8 :     return 191 ;        break ; //器
        case 0xe382a8 :     return 192 ;        break ; //エ
        case 0xe382a6 :     return 193 ;        break ; //ウ
        case 0xe99689 :     return 194 ;        break ; //閉
        case 0xe5a19e :     return 195 ;        break ; //塞
        case 0xe4bd8e :     return 196 ;        break ; //低
        case 0xe9818e :     return 197 ;        break ; //過
        case 0xe59ca7 :     return 198 ;        break ; //圧
        case 0xe58aa0 :     return 199 ;        break ; //加
        case 0xe5b8af :     return 200 ;        break ; //帯
        case 0xe5a0b4 :     return 201 ;        break ; //場
        case 0xe68980 :     return 202 ;        break ; //所
        case 0xe7a9ba :     return 203 ;        break ; //空
        case 0xe6bc8f :     return 204 ;        break ; //漏
        case 0xe3828c :     return 205 ;        break ; //れ
        case 0xe7af84 :     return 206 ;        break ; //範
        case 0xe59bb2 :     return 207 ;        break ; //囲
        case 0xe8b685 :     return 208 ;        break ; //超
        case 0xe7b7a9 :     return 209 ;        break ; //緩
        case 0xe5b7b1 :     return 210 ;        break ; //己
        case 0xe5a4b1 :     return 211 ;        break ; //失
        case 0xe69597 :     return 212 ;        break ; //敗
        case 0xe382b8 :     return 213 ;        break ; //ジ
        case 0xe688bb :     return 214 ;        break ; //戻
        case 0xe980b2 :     return 215 ;        break ; //進
        case 0xe38281 :     return 216 ;        break ; //め
        case 0xe5b7a6 :     return 217 ;        break ; //左
        case 0xe58fb3 :     return 218 ;        break ; //右
        case 0xe382ba :     return 219 ;        break ; //ズ
        case 0xe584aa :     return 220 ;        break ; //優
        case 0xe58588 :     return 221 ;        break ; //先
        case 0xe4ba8b :     return 222 ;        break ; //事
        case 0xe9a085 :     return 223 ;        break ; //項
        case 0xe3839c :     return 224 ;        break ; //ボ
        case 0xe784a1 :     return 225 ;        break ; //無
        case 0xe99fb3 :     return 226 ;        break ; //音
        case 0xe38380 :     return 227 ;        break ; //ダ
        case 0xe6a899 :     return 228 ;        break ; //標
        case 0xe6ba96 :     return 229 ;        break ; //準
        case 0xe38386 :     return 230 ;        break ; //テ
        case 0xe382ad :     return 231 ;        break ; //キ
        case 0xe3838e :     return 232 ;        break ; //ノ
        case 0xe68caf :     return 233 ;        break ; //振
        case 0xe5b985 :     return 234 ;        break ; //幅
        case 0xe59ca8 :     return 235 ;        break ; //在
        case 0xe9a086 :     return 236 ;        break ; //順
        case 0xe5ba8f :     return 237 ;        break ; //序
        case 0xe7bdae :     return 238 ;        break ; //置
        case 0xe5878d :     return 239 ;        break ; //凍
        case 0xe7b590 :     return 240 ;        break ; //結
        case 0xe382bd :     return 241 ;        break ; //ソ
        case 0xe69687 :     return 242 ;        break ; //文
        case 0xe5ad97 :     return 243 ;        break ; //字
        case 0xe799bd :     return 244 ;        break ; //白
        case 0xe6ac84 :     return 245 ;        break ; //欄
        case 0xe6b688 :     return 246 ;        break ; //消
        case 0xe58ebb :     return 247 ;        break ; //去
        case 0xe382bb :     return 248 ;        break ; //セ
        case 0xe58f96 :     return 249 ;        break ; //取
        case 0xe7acac :     return 250 ;        break ; //第
        case 0xe5b7a5 :     return 251 ;        break ; //工
        case 0xe88db7 :     return 252 ;        break ; //荷
        case 0xe38396 :     return 253 ;        break ; //ブ
        case 0xe5b9b4 :     return 254 ;        break ; //年
        case 0xe69c88 :     return 255 ;        break ; //月
        case 0xe794b7 :     return 256 ;        break ; //男
        case 0xe5a5b3 :     return 257 ;        break ; //女
        case 0xe5908d :     return 258 ;        break ; //名
        case 0xe795aa :     return 259 ;        break ; //番
        case 0xe680a7 :     return 260 ;        break ; //性
        case 0xe588a5 :     return 261 ;        break ; //別
        case 0xe7b6ad :     return 262 ;        break ; //維
        case 0xe68c81 :     return 263 ;        break ; //持
        case 0xe68abc :     return 264 ;        break ; //押
        case 0xe68c87 :     return 265 ;        break ; //指
        case 0xe4bba4 :     return 266 ;        break ; //令
        case 0xe98182 :     return 267 ;        break ; //遂
        case 0xe591bc :     return 268 ;        break ; //呼
        case 0xe590b8 :     return 269 ;        break ; //吸
        case 0xe382b3 :     return 270 ;        break ; //コ
        case 0xe5a49a :     return 271 ;        break ; //多
        case 0xe8a3bd :     return 272 ;        break ; //製
        case 0xe59381 :     return 273 ;        break ; //品
        case 0xe383a7 :     return 274 ;        break ; //ョ
        case 0xe58d94 :     return 275 ;        break ; //協
        case 0xe382b4 :     return 276 ;        break ; //ゴ
        case 0xe382a7 :     return 277 ;        break ; //ェ
        case 0xe69c89 :     return 278 ;        break ; //有
        case 0xe8b2ac :     return 279 ;        break ; //責
        case 0xe4bbbb :     return 280 ;        break ; //任
        case 0xe4bc9a :     return 281 ;        break ; //会
        case 0xe7a4be :     return 282 ;        break ; //社
        case 0xe3809c :     return 283 ;        break ; //〜
        case 0xe381a4 :     return 284 ;        break ; //つ
        case 0xe8a880 :     return 285 ;        break ; //言
        case 0xe8aa9e :     return 286 ;        break ; //語
        case 0xe59bbd :     return 287 ;        break ; //国
        case 0xe88bb1 :     return 288 ;        break ; //英
        case 0xe9ab98 :     return 289 ;        break ; //高
        case 0xe794bb :     return 290 ;        break ; //画
        case 0xe8b3aa :     return 291 ;        break ; //質
        case 0xe88488 :     return 292 ;        break ; //脈
        case 0xe8a180 :     return 293 ;        break ; //血
        case 0xe7b5b1 :     return 294 ;        break ; //統
        case 0xe8a888 :     return 295 ;        break ; //計
        case 0xe6bf83 :     return 296 ;        break ; //濃
        case 0xe7b8ae :     return 297 ;        break ; //縮
        case 0xe6938d :     return 298 ;        break ; //操
        case 0xe8a8ba :     return 299 ;        break ; //診
        case 0xe696ad :     return 300 ;        break ; //断
        case 0xe58e9f :     return 301 ;        break ; //原
        case 0xe8a898 :     return 302 ;        break ; //記
        case 0xe98cb2 :     return 303 ;        break ; //録
        case 0xe795b0 :     return 304 ;        break ; //異
        case 0xe5b8b8 :     return 305 ;        break ; //常
        case 0xe5b9b3 :     return 306 ;        break ; //平
        case 0xe59d87 :     return 307 ;        break ; //均
        case 0xe79bae :     return 308 ;        break ; //目
        case 0xe8b2a0 :     return 309 ;        break ; //負
        case 0xe38288 :     return 310 ;        break ; //よ
        case 0xe69c80 :     return 311 ;        break ; //最
        case 0xe695b0 :     return 312 ;        break ; //数
        case 0xe896ac :     return 313 ;        break ; //薬
        case 0xe589a4 :     return 314 ;        break ; //剤
        case 0xe580a4 :     return 315 ;        break ; //値
        case 0xe9878d :     return 316 ;        break ; //重
        case 0xe68a95 :     return 317 ;        break ; //投
        case 0xe4b88e :     return 318 ;        break ; //与
        case 0xe9878f :     return 319 ;        break ; //量
        case 0xe5928c :     return 320 ;        break ; //和
        case 0xe6bd85 :     return 321 ;        break ; //潅
        case 0xe6b0b4 :     return 322 ;        break ; //水
        case 0xe6bbb4 :     return 323 ;        break ; //滴
        case 0xe6b2bb :     return 324 ;        break ; //治
        case 0xe79982 :     return 325 ;        break ; //療
        case 0xe59bb3 :     return 326 ;        break ; //図
        case 0xe58f82 :     return 327 ;        break ; //参
        case 0xe88083 :     return 328 ;        break ; //考
        case 0xe5a496 :     return 329 ;        break ; //外
        case 0xe6848f :     return 330 ;        break ; //意
        case 0xe3818a :     return 331 ;        break ; //お
        case 0xe5be85 :     return 332 ;        break ; //待
        case 0xe381a1 :     return 333 ;        break ; //ち
        case 0xe5ae8c :     return 334 ;        break ; //完
        case 0xe381b9 :     return 335 ;        break ; //べ
        case 0xe69bb4 :     return 336 ;        break ; //更
        case 0xe7b69a :     return 337 ;        break ; //続
        case 0xe3839f :     return 338 ;        break ; //ミ
        case 0xe38394 :     return 339 ;        break ; //ピ
        case 0xe38398 :     return 340 ;        break ; //ヘ
        case 0xe6b6b2 :     return 341 ;        break ; //液
        case 0xe58ab9 :     return 342 ;        break ; //効
        case 0xe4b889 :     return 343 ;        break ; //三
        case 0xe6aeb5 :     return 344 ;        break ; //段
        case 0xe9a0bb :     return 345 ;        break ; //頻
        case 0xe5be90 :     return 346 ;        break ; //徐
        case 0xe6aca0 :     return 347 ;        break ; //欠
        case 0xe4bc91 :     return 348 ;        break ; //休
        case 0xe4b88d :     return 349 ;        break ; //不
        case 0xe695b4 :     return 350 ;        break ; //整
        case 0xe5ada6 :     return 351 ;        break ; //学
        case 0xe7bf92 :     return 352 ;        break ; //習
        case 0xe5bcb1 :     return 353 ;        break ; //弱
        case 0xe3818e :     return 354 ;        break ; //ぎ
        case 0xe686b6 :     return 355 ;        break ; //憶
        case 0xe8a385 :     return 356 ;        break ; //装
        case 0xe8a68b :     return 357 ;        break ; //見
        case 0xe4bf9d :     return 358 ;        break ; //保
        case 0xe5ad98 :     return 359 ;        break ; //存
        case 0xe38193 :     return 360 ;        break ; //こ
        case 0xe69cac :     return 361 ;        break ; //本
        case 0xe5bd93 :     return 362 ;        break ; //当
        case 0xe8a68f :     return 363 ;        break ; //規
        case 0xe7aea1 :     return 364 ;        break ; //管
        case 0xe382a1 :     return 365 ;        break ; //ァ
        case 0xe69da5 :     return 366 ;        break ; //来
        case 0xe6908d :     return 367 ;        break ; //損
        case 0xe582b7 :     return 368 ;        break ; //傷
        case 0xe999a4 :     return 369 ;        break ; //除
        case 0xe9babb :     return 370 ;        break ; //麻
        case 0xe98594 :     return 371 ;        break ; //酔
        case 0xe599b4 :     return 372 ;        break ; //噴
        case 0xe8a7a3 :     return 373 ;        break ; //解
        case 0xe69bbf :     return 374 ;        break ; //替
        case 0xe98193 :     return 375 ;        break ; //道
        case 0xe8a9b0 :     return 376 ;        break ; //詰
        case 0xe99d9e :     return 377 ;        break ; //非
        case 0xe382b5 :     return 378 ;        break ; //サ
        case 0xe587a6 :     return 379 ;        break ; //処
        case 0xe79086 :     return 380 ;        break ; //理
        case 0xe9968b :     return 381 ;        break ; //開
        case 0xe5a78b :     return 382 ;        break ; //始
        case 0xe7bea9 :     return 383 ;        break ; //義
        case 0xe78ab6 :     return 384 ;        break ; //状
        case 0xe6858b :     return 385 ;        break ; //態
        case 0xe381b0 :     return 386 ;        break ; //ば
        case 0xe38289 :     return 387 ;        break ; //ら
        case 0xe6aca1 :     return 388 ;        break ; //次
        case 0xe381b8 :     return 389 ;        break ; //へ
        case 0xe8adb7 :     return 390 ;        break ; //護
        case 0xe98081 :     return 391 ;        break ; //送
        case 0xe58f8d :     return 392 ;        break ; //反
        case 0xe5bf9c :     return 393 ;        break ; //応
        case 0xe58299 :     return 394 ;        break ; //備
        case 0xe79fad :     return 395 ;        break ; //短
        case 0xe68b8d :     return 396 ;        break ; //拍
        case 0xe88793 :     return 397 ;        break ; //臓
        case 0xe79785 :     return 398 ;        break ; //病
        case 0xe6b3a8 :     return 399 ;        break ; //注
        case 0xe99bbb :     return 400 ;        break ; //電
        case 0xe6b1a0 :     return 401 ;        break ; //池
        case 0xe8a9b3 :     return 402 ;        break ; //詳
        case 0xe7b4b0 :     return 403 ;        break ; //細
        case 0xe5bdb9 :     return 404 ;        break ; //役
        case 0xe8baab :     return 405 ;        break ; //身
        case 0xe59fba :     return 406 ;        break ; //基
        case 0xe69bb2 :     return 407 ;        break ; //曲 --
        case 0xe98185 :     return 408 ;        break ; //遅
        case 0xe68ea5 :     return 409 ;        break ; //接
        case 0xe6b581 :     return 410 ;        break ; //流
        case 0xe5be8c :     return 411 ;        break ; //後
        case 0xe5bc95 :     return 412 ;        break ; //引
        case 0xe3838f :     return 413 ;        break ; //ハ
        case 0xe6a0bc :     return 414 ;        break ; //格
        case 0xe5ad90 :     return 415 ;        break ; //子
        case 0xe889b2 :     return 416 ;        break ; //色
        case 0xe69fbb :     return 417 ;        break ; //査
        case 0xe68890 :     return 418 ;        break ; //成
        case 0xe58faf :     return 419 ;        break ; //可
        case 0xe58aa9 :     return 420 ;        break ; //助
        case 0xe8aaad :     return 421 ;        break ; //読
        case 0xe8bebc :     return 422 ;        break ; //込
        case 0xe381bf :     return 423 ;        break ; //み
        case 0xe883bd :     return 424 ;        break ; //能
        case 0xe590a6 :     return 425 ;        break ; //否
        case 0xe697a7 :     return 426 ;        break ; //旧
        case 0xe38198 :     return 427 ;        break ; //じ
        case 0xe582be :     return 428 ;        break ; //傾
        case 0xe59091 :     return 429 ;        break ; //向
        case 0xe99da2 :     return 430 ;        break ; //面
        case 0xe5889d :     return 431 ;        break ; //初
        case 0xe69c9f :     return 432 ;        break ; //期

        default:
            //printf ("cant from Japan *(s)=%x \n" , testcode ) ;
            return 0 ;//没找到字,不正常返回.
            break;
        }
    }//end unsigned JapenCodeTransform(long testcode){

    unsigned KoreaCodeTransform(unsigned long testcode){//retrun Korea and gw_Korea2 index
	switch(testcode){
		case 0xeca285:         return 1;      break;    //종
		case 0xeba38c:         return 2;      break;    //료
		case 0xec84a4:         return 3;      break;    //설
		case 0xeca095:         return 4;      break;    //정
		case 0xeba6ac:         return 5;      break;    //리
		case 0xeb939c:         return 6;      break;    //드
		case 0xeab28c:         return 7;      break;    //게
		case '인':         return 8;      break;    //인
		case '심':         return 9;      break;    //심
		case '박':         return 10;      break;    //박
		case '조':         return 11;      break;    //조
		case '율':         return 12;      break;    //율
		case '기':         return 13;      break;    //기
		case '탐':         return 14;      break;    //탐
		case '지':         return 15;      break;    //지

		default:
                //printf ("cant from Japan *(s)=%x \n" , testcode ) ;
			return 0 ;//没找到字,不正常返回.
			break;
	}
}
