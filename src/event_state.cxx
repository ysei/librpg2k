#include "rpg2k/define/define.hxx"

const char* rpg2k::define::event_state =
"array1d event_state \n"
"{\n"
"	[11]: int map_id;\n"
"	[12]: int x = 0;\n"
"	[13]: int y = 0;\n"
"\n"
"	[21]: int defDir;\n"
"	[22]: int talkDir;\n"
"	[23]: int dummy;\n"
"	[24]: int dummy;\n"
"\n"
"	[31]: int action = 0;\n"
"	[32]: int freq;\n"
"	[33]: int start_type = 0;\n"
"	[34]: int priority_type;\n"
"	[35]: int dummy;\n"
"	[36]: int anime_type = 0;\n"
"	[37]: int speed = 3;\n"
"\n"
"	[41]: array1d move\n"
"	{\n"
"		[11]: int length = 0;\n"
"		[12]: binary data;\n"
"\n"
"		[21]: bool repeat = true;\n"
"		[22]: bool pass = false;\n"
"	};\n"
"	[43]: int dummy;\n"
"	[46]: bool isTrans = false;\n"
"\n"
"	// [51]:\n"
"	[52]: int counter = 0;\n"
"	[53]: int dummy;\n"
"	[54]: int dummy;\n"
"\n"
"	[71]: int dummy;\n"
"	[72]: int dummy;\n"
"	[73]: string char_set;\n"
"	[74]: int char_set_pos = 0;\n"
"	[75]: int char_setPat = 1;\n"
"\n"
"	[81]: int dummy;\n"
"	[82]: int dummy;\n"
"	[83]: int dummy;\n"
"\n"
"	[101]: int dummy;\n"
"	[103]: int vehicle = 0;\n"
"	[108]: array1d dummy\n"
"	{\n"
"		[1]: int dummy;\n"
"	};\n"
"\n"
"	[121]: int dummy;\n"
"\n"
"	[131]: int dummy;\n"
"	[132]: binary dummy;\n"
"};\n"
;
