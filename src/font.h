/* font.h */
#include "unordered_map"

//8x8 font can be combined into single 64b bitmap.
typedef uint64_t Glyph;

#define NUM_GLYPHS 64u

enum GlyphID {
	//"Special" or punctuation
	G_UNKNOWN, //Renders the unknown-char box.
	G_SPACE, //' '
	G_COMMA, //','
	G_FULLSTOP, //'.'
	G_COLON, //':'
	G_SEMICOLON, //';'
	G_HASHTAG, //'#'
	G_LPAREN, //'('
	G_RPAREN, //')'
	G_LSQBRC, //'['
	G_RSQBRC, //']'
	G_LBRACE, //'{'
	G_RBRACE, //'}'
	G_EXCLAIM, //'!'
	G_QUERY, //'?'
	G_QUOTE, //'"'
	G_APOSTROPHE, //'''
	G_ADD, //'+'
	G_SUB, //'-'
	G_STAR, //'*'
	G_EQU, //'='
	G_RSLASH, //'/'
	G_LSLASH, //'\'
	G_LT, //'<'
	G_GT, //'>'
	G_UNDERSCORE, //'_'
	G_PIPE, //'|'
	G_AMPERSAND, //'&'
	G_PERCENT, //'%'

	//Lowercase letters
	G_a, //'a'
	G_b, //'b'
	G_c, //'c'
	G_d, //'d'
	G_e, //'e'
	G_f, //'f'
	G_g, //'g'
	G_h, //'h'
	G_i, //'i'
	G_j, //'j'
	G_k, //'k'
	G_l, //'l'
	G_m, //'m'
	G_n, //'n'
	G_o, //'o'
	G_p, //'p'
	G_q, //'q'
	G_r, //'r'
	G_s, //'s'
	G_t, //'t'
	G_u, //'u'
	G_v, //'v'
	G_w, //'w'
	G_x, //'x'
	G_y, //'y'
	G_z, //'z'

	//Uppercase letters
	G_A, //'A'
	G_B, //'B'
	G_C, //'C'
	G_D, //'D'
	G_E, //'E'
	G_F, //'F'
	G_G, //'G'
	G_H, //'H'
	G_I, //'I'
	G_J, //'J'
	G_K, //'K'
	G_L, //'L'
	G_M, //'M'
	G_N, //'N'
	G_O, //'O'
	G_P, //'P'
	G_Q, //'Q'
	G_R, //'R'
	G_S, //'S'
	G_T, //'T'
	G_U, //'U'
	G_V, //'V'
	G_W, //'W'
	G_X, //'X'
	G_Y, //'Y'
	G_Z, //'Z'

	//Digits
	G_0, //'0'
	G_1, //'1'
	G_2, //'2'
	G_3, //'3'
	G_4, //'4'
	G_5, //'5'
	G_6, //'6'
	G_7, //'7'
	G_8, //'8'
	G_9, //'9'
};

const std::unordered_map<char, GlyphID> specialChars = {
	{' ', G_SPACE},
	{',', G_COMMA},
	{'.', G_FULLSTOP},
	{':', G_COLON},
	{';', G_SEMICOLON},
	{'#', G_HASHTAG},
	{'(', G_LPAREN},
	{')', G_RPAREN},
	{'[', G_LSQBRC},
	{']', G_RSQBRC},
	{'{', G_LBRACE},
	{'}', G_RBRACE},
	{'!', G_EXCLAIM},
	{'?', G_QUERY},
	{'"', G_QUOTE},
	{'\'', G_APOSTROPHE},
	{'+', G_ADD},
	{'-', G_SUB},
	{'*', G_STAR},
	{'=', G_EQU},
	{'/', G_RSLASH},
	{'\\', G_LSLASH},
	{'<', G_LT},
	{'>', G_GT},
	{'_', G_UNDERSCORE},
	{'|', G_PIPE},
	{'&', G_AMPERSAND},
	{'%', G_PERCENT},
};


namespace font {

	extern const Glyph glyphs[NUM_GLYPHS];

	static GlyphID asciiToGEnum(char ascii) {
		//Maps ascii to the enum.
		//Check ranges;
		if ((ascii >= '0') && (ascii <= '9')) {return (GlyphID)(G_0 + (ascii - '0'));}
		if ((ascii >= 'a') && (ascii <= 'z')) {return (GlyphID)(G_a + (ascii - 'a'));}
		if ((ascii >= 'A') && (ascii <= 'Z')) {return (GlyphID)(G_A + (ascii - 'A'));}

		//Else, must be special char;
		auto it = specialChars.find(ascii);
		if (it != specialChars.end()) {return it->second;}

		//Must be unknown char;
		return G_UNKNOWN;
	}

	static const Glyph& getGlyph(char ascii) {
		return glyphs[asciiToGEnum(ascii)];
	}

}
