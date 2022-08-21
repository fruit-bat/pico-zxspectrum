fixMozillaZIndex=true; //Fixes Z-Index problem
// with Mozilla browsers but causes odd scrolling problem, toggle to see if it helps
_menuCloseDelay=500;
_menuOpenDelay=150;
_subOffsetTop=20;
_subOffsetLeft=-2;
horizontalMenuDelay="true"; 	 //true
forgetClickValue="true"; 		   //true
//align="center";



with(menuStyle=new mm_style()){
bgimage="whitex100.gif";
fontfamily="Arial";
//fontsize=14; //90%
fontstyle="normal";
fontweight="bold";
itemheight=26;  // tab height
itemwidth=100;   // tab width
offcolor="black";
oncolor="white";
openonclick=0;
//subimagepadding=30;
overbgimage="blackx100.gif";
}

// sub menus ****************************************************************************
with(submenuStyle=new mm_style()){
styleid=1;
align="center";
itemwidth=192;
bgimage="tab_subred_bk.gif";
//bgimage="tab_sub_1.gif";
fontfamily="Verdana, Tahoma, Arial";
//fontsize=11;  //90%
fontstyle="normal";
fontweight="bold";
//itemheight=;
offcolor="#ffffff";
oncolor="black";
openonclick=0;
padding=4;
bgcolor="red";
//separatorimage="tab_subback_sep_a.gif";
//separatorsize=3;
}


//  sub menu !!!!!
with(sub_2=new mm_style()){
styleid=1;
align="center";
itemwidth=192;
//bgimage="tab_subback_bk.gif";
bgimage="tab_sub_3.gif";
fontfamily="Verdana, Tahoma, Arial";
//fontsize=11;  //90%
fontstyle="normal";
fontweight="bold";
//itemheight=;
//offbgcolor="black";
offcolor="#ffffff";
oncolor="gray";
openonclick=0;
padding=4;
bgcolor="red";
//separatorimage="tab_subback_sep_a.gif";
//separatorsize=3;
}

 //  sub menu !!!!!
with(sub_1=new mm_style()){
styleid=1;
align="center";
itemwidth=192;
//bgimage="tab_subback_bk.gif";
bgimage="tab_sub_2.gif";
fontfamily="Verdana, Tahoma, Arial";
//fontsize=11;  //90%
fontstyle="normal";
fontweight="bold";
//itemheight=;
//offbgcolor="black";
offcolor="#ffffff";
oncolor="gray";
openonclick=0;
padding=4;
bgcolor="red";
//separatorimage="tab_subback_sep_a.gif";
//separatorsize=3;
}


// *********************************************************************
with(milonic=new menuname("Main Menu")){
alwaysvisible=1;
orientation="horizontal";
position="relative"; // added
screenposition="left";
style=menuStyle;
//top=165; // default set as 135
aI("text=Home;title=Home;align=center;keepalive=0;showmenu=home;url=index.html;status=Achive Home page;");
aI("align=center;keepalive=0;showmenu=what_is;text=What is ...;url=index_what_is.html");
aI("align=center;keepalive=0;text=Hardware;url=ace_hardware.html;");
aI("align=center;keepalive=0;showmenu=archive;text=Software;url=software_index.html");
aI("align=center;keepalive=0;text=Documents;url=documents_index.html");
aI("align=center;keepalive=0;showmenu=programming;text=Programming;");
aI("align=center;keepalive=0;showmenu=emulators;text=Emulators;url=index_emulators.html;");
aI("align=center;keepalive=0;url=index_faqs.html;text=FAQs;");
aI("align=center;keepalive=0;url=links.html;text=Links;");
}
//*************************************************************************

with(milonic=new menuname("home")){
orientation="vertical";
align="left";
top=171;
style=submenuStyle;
//aI("text=Home;url=index.html;title=Jupiter Ace Archive");
//aI("text=Forums;url=http://www.forums1.jupiter-ace.co.uk;");
aI("text=Flickr Ace Image Pool;url=http://www.flickr.com/groups/jupiterace");
aI("text=YouTube Channel;url=http://www.youtube.com/user/JupiterAceArchive#g/u");
aI("text=Ace Search ;url=/search/search.php?adv=1;title=Jupiter Ace Archive Search");
aI("text=Whats New;url=whatsnew.html;");
aI("text=Contact Us;url=contactme.html;") ;
aI("text=Wanted !;url=wanted_list.html;") ;
//aI("text=Credits;url=credits.html;");
//aI("text=Site Map;url=sitemap.html;");
//aI("text=Site Search;url=search_site.html;");
//aI("text=Site Stats;url=site_stats.html;");
}

with(milonic=new menuname("archive")){
orientation="vertical";
align="left";
top=171;
style=submenuStyle;
 aI("text=Software titles 'A' to 'B';url=software_index_a_b.html;title=Software Archive");
 aI("text=Software titles 'C' to 'D';url=software_index_c_d.html;title=Software Archive");
 aI("text=Software titles 'E' to 'F';url=software_index_e_f.html;title=Software Archive");
 aI("text=Software titles 'G' to 'H';url=software_index_g_h.html;title=Software Archive");
 aI("text=Software titles 'I' to 'J';url=software_index_i_j.html;title=Software Archive");
 aI("text=Software titles 'K' to 'L';url=software_index_k_l.html;title=Software Archive");
 aI("text=Software titles 'M' to 'N';url=software_index_m_n.html;title=Software Archive");
 aI("text=Software titles 'O' to 'P';url=software_index_o_p.html;title=Software Archive");
 aI("text=Software titles 'Q' to 'R';url=software_index_q_r.html;title=Software Archive");
 aI("text=Software titles 'S';url=software_index_s.html;title=Software Archive");
 aI("text=Software titles 'T';url=software_index_t.html;title=Software Archive");
 aI("text=Software titles 'U' to 'V';url=software_index_u_v.html;title=Software Archive");
 aI("text=Software titles 'W' to 'X';url=software_index_w_x.html;title=Software Archive");
 aI("text=Software titles 'Y' to 'Z';url=software_index_y_z.html;title=Software Archive");
 aI("text=Magazine Software;url=software_index_mag.html;title=Software from Magazines");
 aI("text=SW House Index;url=index_SWHouse_index.html;");
 aI("text=ROM Downloads;url=roms.html;title=Ace FORTH ROM imges");
}



with(milonic=new menuname("what_is")){
orientation="vertical";
align="left";
top=171;
style=submenuStyle;
aI("text=..Forth;url=whatisforth.html;");

aI("text=..A Jupiter ACE;url=whatisanace.html;");
aI("text=..Jupiter Ace4000;url=ace4000.html;");
aI("text=..Jupiter ACE16+;url=jupiterace16.html;");
aI("text=..Turnkey System (TKS);url=jupiterace-tks.html;");
aI("text=..Jupiter Cantab;url=what_is_jupiter_cantab.html;");
aI("text=..Boldfield Computing;url=mia.html;");
 aI("text=..Minstrel 4th;url=what_is_minstrel.html;");
 aI("text=..Jester ACE;url=what_is_jesterACE.html;");
//aI("text=..AMI V2;url=hardware_ami-V2_index.html;");
//aI("text=Java Emulator;url=zzace_09.html;");
// aI("text=board images;url=ace_board.html;");
// aI("text=Time Line;url=timeline.html;");
}



// documents ########################################################################
with(milonic=new menuname("documents")){
orientation="vertical";
align="left";
top=171;
style=submenuStyle;
aI("align=center;keepalive=0;showmenu=hard_schematics;text=Ace Schematics;");
aI("align=center;keepalive=0;showmenu=doc_manuals;text=User Manuals;");
aI("align=center;keepalive=0;showmenu=doc_hard_manuals;text=Hardware Manuals;");
aI("align=center;keepalive=0;showmenu=doc_software_manuals;text=Software Manuals;");
aI("align=center;keepalive=0;showmenu=doc_p_reviews;text=Ace Press Reviews;title=Jupiter Ace Press Reviews;");
aI("align=center;keepalive=0;showmenu=doc_hardware_reviews;text=Hardware Reviews;title=Hardware Reviews;");
aI("align=center;keepalive=0;showmenu=doc_software_reviews;text=Software Reviews;title=Software Reviews;");
aI("align=center;keepalive=0;showmenu=doc_news;text=Ace Press News ;title=Jupiter Ace Press News;");
aI("align=center;keepalive=0;showmenu=doc_features;text=Ace Press Features;title=Jupiter Ace Features;");
aI("align=center;keepalive=0;showmenu=doc_letters;text=User Letters in Press;title=Jupiter Ace User letters in press;");
aI("text=Sales Brochures &amp; Adverts;url=adverts.html;title=All Adverts Index;");
//aI("align=center;keepalive=0;showmenu=doc_adverts;text=Jupiter Ace Adverts;title=Jupiter Ace Adverts in the Press;");



aI("align=center;keepalive=0;showmenu=doc_books;text=Books on Forth &amp; Ace;title=Books about Forth &amp; The Ace;");
aI("align=center;keepalive=0;text=User Fanzines;url=aceusers.html;");
aI("align=center;keepalive=0;showmenu=doc_fortharticles;text=Articles from Magazines;title=Forth articles from magazines;");
//aI("text=Videos index ;url=index_videos.html;title=Videos index page;");
}

with(milonic=new menuname("doc_manuals")){
orientation="vertical";
align="left";
top=180;
style=sub_1;
aI("text=First Edition manual;url=usermanual.html#ace_manuals;title=Manual as PDF;;");
aI("text=Second Edition manual;url=usermanual.html#ace_manuals;title=Manual as PDF;");
aI("text=Ace4000 Edition manual;url=usermanual.html#ace_manuals;title=Manual as PDF;");
aI("text=German  Edition manual;url=usermanual.html#ace_manuals;title=Manual as PDF;");
aI("text=French Edition manual;url=usermanual.html#ace_manuals;title=Manual as PDF;");
aI("text=FORTH Bridge manual;url=usermanual.html#ace_manuals;title=Manual as PDF;");
aI("text=AceFORTH Reference Card;url=usermanual.html#ace_manuals;");
aI("text=Screen Lay Out Card;url=usermanual.html#ace_manuals;title=Manual as PDF;");
aI("text=ebook project;url=usermanual.html#ace_manuals;");
}

with(milonic=new menuname("doc_hard_manuals")){
orientation="vertical";
align="left";
top=195;
style=sub_1;
aI("text=Cantab RAM Pack manual;url=usermanual.html#hardware_manuals;title=Manual as PDF;");
aI("text=Memotech Keyboard manual;url=usermanual.html#hardware_manuals;title=Manual as PDF;");
aI("text=EME Sound Board  manual;url=usermanual.html#hardware_manuals;title=Manual as PDF;");
}

 with(milonic=new menuname("doc_software_manuals")){
orientation="vertical";
align="left";
top=225;
style=sub_1;
aI("text=Boldfeild Database Manual;url=usermanual.html#software_manuals;title=Database Manual as PDF;");
aI("text=Boldfeild Spreadsheet Manual;url=usermanual.html#software_manuals;title=Spreadsheet Manual as PDF;");
aI("text=Boldfeild Utilites Manual;url=usermanual.html#software_manuals;title=Utilites Manual as PDF;");
aI("text=Boldfeild Assembler Manual;url=usermanual.html#software_manuals;title=Assembler / Diss Assembler Manual as PDF;");
}

with(milonic=new menuname("doc_p_reviews")){
orientation="vertical";
align="left";
top=235;
style=sub_1;
aI("text=All Reviews;url=reviews.html#ace_hardware_reviews;title=Ace & Hardware Reviews Index;");
aI("align=center;keepalive=0;showmenu=doc_p_reviews1983;text=Ace Press Reviews 1983;title=Press Reviews from 1983;");
aI("align=center;keepalive=0;showmenu=doc_p_reviews1982;text=Ace Press Reviews 1982;title=Press Reviews from 1982;");
}

with(milonic=new menuname("doc_p_reviews1983")){
orientation="vertical";
align="left";
top=265;
style=sub_2;
aI("text=Electronic pratique Nov'83;url=review_e_pratique.html;title=from Electronic pratique November'83;");
aI("text=Radio Bulletin Oct'83;url=review_radiobulletin.html;title=from Dutch Radio Bulletin October'83;");
aI("text=Creative Computing July'83;url=review_cc.html;title=from Creative Computing V9 No7 July'83;");
aI("text=Computing Today May'83;url=review_ComputingToDay8305.html;title=from Computing Today May'83;");
aI("text=PC World Jan'83;url=review_pcworld_8301.html;title=from Personal Computer World Jan'83;");
}

with(milonic=new menuname("doc_p_reviews1982")){
orientation="vertical";
align="left";
top=275;
style=sub_2;
aI("text=Popular Computer Weekly Sep'82;url=review_pop.html;title=from Popular Computing Weekly September'82;");
aI("text=Electronics & Computer Nov'82;url=review_ec.htm;title=from Electronics & Computing November'82.;");
aI("text=Your Computer Nov'82;url=review_yc.html;title=from Your Computer November '82;");
aI("text=Computer Answers Nov/Dec'82;url=review_ComputerAnswers8211.html;title=from Computer Answers Nov/Dec '82;");
aI("text=Personal Computing ToDay Dec'82;url=review_PCToday8212.html;title=from Personal Computing ToDay Dec '82;");
}

with(milonic=new menuname("doc_hardware_reviews")){
orientation="vertical";
align="left";
top=295;
style=sub_1;
aI("text=All Reviews;url=reviews.html#ace_software_reviews;title=Ace & Hardware Reviews Index;");
aI("text=ADS Centronics Card Review;url=hardware_ads_centronics_review.html;title=ADS Centronics Interface Review;");
aI("text=EME Sound Card Review;url=review_soundcard.html;title=EME Sound Card Review;");
}

with(milonic=new menuname("doc_software_reviews")){
orientation="vertical";
align="left";
top=265;
style=sub_1;
aI("text=All Software Reviews;url=reviews.html#ace_software_reviews;title=Ace & Hardware Reviews Index;");
aI("text=GAMEPLAY Reviews;url=review_PCN830616p50.html;title=Gameplay software reviews from Personal Computer News '83;");
}

with(milonic=new menuname("doc_news")){
orientation="vertical";
align="left";
top=300;
style=sub_1;
aI("text=All News index;url=reviews.html#news;title=All News Index;");
aI("align=center;keepalive=0;showmenu=doc_news_1983;text=Ace Press News 1983;title=Press News from 1983;");
aI("align=center;keepalive=0;showmenu=doc_news_1982;text=Ace Press News 1982;title=Press News from 1982;");
}

with(milonic=new menuname("doc_news_1983")){
orientation="vertical";
align="left";
top=290;
style=sub_2;
aI("text=Cantab Creditors Appointed;url=news_PCN831124_02.html;title=Cantab Creditors Appointed Personal Computer News Nov ;");
aI("text=Cantab Sale;url=news_PCWeekly831017.html;title=Cantab crashes from Personal Computer News Nov;");
aI("text=Ace company for sale;url=news_831122-HCW.html;title=Ace company for sale Home Computing Weekly Nov;");
aI("text=Jupiter hits the Dust;url=news_Pop_No44_83.html;title=Jupiter hits the Dust from Popular Computing Weekly Nov;");
aI("text=Cantab crashes;url=news_pcnews831103_2.html;title=Cantab crashes from Personal Computer News Nov;");
aI("text=Disk mix for Ace;url=news_PCN830901[JetDiskNews].html;title=Disk mix for Ace from Personal Computer News Sept;");
aI("text=ACE is now Ace 16+;url=news_PracComputing8308.html;title=JUPITER ACE is now the Jupiter Ace 16+ from Practical Computing August;");
aI("text=Jupiter Ace Plays its Ace!;url=news_PCWorld-8308_page_102.html;title=Jupiter Ace Plays its Ace! from Personal Computing World August;");
aI("text=Ace Software;url=news_Pop_No27_83.html;title=Ace Software from Popular Computing Weekly July;");
aI("text=Playing the Ace;url=review_news_PNC_April0183.html;title=Playing the Ace from Personal Computer News April;");
aI("text=High Street dealers to hold all the Aces;url=news_YC_Jan_83.html;title=High Street dealers to hold all the Aces from Your Computer Jan;");
}

with(milonic=new menuname("doc_news_1982")){
orientation="vertical";
align="left";
top=290;
style=sub_2;
aI("text=Ace comes up trumps;url=news_PCWeekly821230.html;title=Ace comes up trumps from Popular computing Weekly Dec;");
aI("text=Ace gets users group;url=news_PCWeekly821216.html;title=Ace gets users group from Popular computing Weekly Dec;");
aI("text=Rival Systems Grow;url=news_ZXComputing-8212-80.html;title=Rival Systems Grow from ZX Computing Dec;");
aI("text=Jupiter Ace Comes FORTH;url=news_pctoday_8211.html;title=Jupiter Ace Comes FORTH from Personal Computing Today Nov;");
aI("text=Cantab talks to MCW;url=news_MicroComputerWeekly.html;title=Jupiter Cantab talks to Micro Computer Weekly from Micro Computer Weekly Nov;");
aI("text=Forth they went together;url=news_8210-WW.html;title=Forth they went together from Wireless World Oct;");
aI("text=Spectrum designers use Forth;url=news_SU_Oct_82.html;title=Spectrum designers use Forth from Sinclair User Oct;");
aI("text=Win an Ace!;url=win_an_ace.html;title=Win an Ace! from Popular Computing Weekly Oct;");
aI("text=Ace goes far faster with Forth;url=news-YourComputer8209-18.html;title=Ace goes far faster with Forth from Your Computer Sept;");
aI("text=spectrum team deal their Ace!;url=news_pcw82072600005.html;title=pectrum team deal their Ace! from Popular Computing Weekly August;");
aI("text=Software Boost;url=news_PCWeekly820331.html;title=Software Boost from Popular computing Weekly March;");
 }

 with(milonic=new menuname("doc_features")){
orientation="vertical";
align="left";
top=340;
style=sub_1;
aI("text=All Features & Reviews;url=reviews.html#features;title=Ace Features Index;");
aI("text=Zigguat Column;url=news_Features_ZigguratPCWeekly830312.html;title=Zigguat Column Cracking eggs with sledgehammers ;");
aI("text=The Home Computer Course;url=review_hcc.html;title=The Home Computer Course;");
aI("text=Ace in Digital Retro Book;url=news_digital_retro_book.html;title=Ace in Digital Retro Book;");
aI("text=Company Profile October;url=news_prac_com_sup.html;title=Company Profile from Practical Computing Special Supplement October 83;");
aI("text=Company Profile July;url=review_profileHCWeekly830737.html;title=Company Profile from Home Computing Weekly July 83;");
aI("text=Buyers Guide;url=reviews_BuyersGuide-HCW.html;title=Buyers Guide from Home Computing Weekly Nov 83;");
}

 with(milonic=new menuname("doc_letters")){
orientation="vertical";
align="left";
top=350;
style=sub_1;
aI("text=All User Letters;url=reviews.html#letters;title=All User letters Index;");
aI("align=center;keepalive=0;showmenu=doc_letters_85;text=User Letters to press '85;title=User Letters to press '85;");
aI("align=center;keepalive=0;showmenu=doc_letters_84;text=User Letters to press '84;title=User Letters to press '84;");
aI("align=center;keepalive=0;showmenu=doc_letters_83;text=User Letters to press '83;title=User Letters to press '83;");
aI("align=center;keepalive=0;showmenu=doc_letters_82;text=User Letters to press '82;title=User Letters to press '82;");
}

 with(milonic=new menuname("doc_letters_85")){
orientation="vertical";
align="left";
top=380;
style=sub_2;
aI("text=Ace of clubs;url=letters_PCWeekly850502.html;title=Ace of clubs letter in Poplar Computing Weekly May'85;");
}

 with(milonic=new menuname("doc_letters_84")){
orientation="vertical";
align="left";
top=380;
style=sub_2;
aI("text=Ace Stockists;url=letters_PCWeekly841108.html;title=Ace Stockists letter in Poplar Computing Weekly Nov'84;");
aI("text=Ace In The Hand?;url=letters_YourComputer8409.html;title=Ace In The Hand? letter in Your Computer Sept'84;");
aI("text=Jupiter Dawn?;url=letters_YourComputer8404.html;title=Jupiter Dawn? letter in Your Computer May'84;");
aI("text=Second Language;url=letters_PCWeekly840429.html;title=Second Language letter in Poplar Computing Weekly April'84;");
aI("text=Ace on Spectrum;url=letters_PCWeekly840419.htm;title=Ace on Spectrum letter in Poplar Computing Weekly April'84;");
aI("text=Ace Enthusiasts;url=letters_PCWeekly840202.html;title=Ace Enthusiasts letter in Poplar Computing Weekly Feb'84;");
}

with(milonic=new menuname("doc_letters_83")){
orientation="vertical";
align="left";
top=410;
style=sub_2;
aI("text=Ace Club;url=letters_PCWeekly831215.html;title=Ace Club letter in Poplar Computing Weekly Dec'83;");
aI("text=Ace User Club;url=letters_PCWeekly831201.html;title=Ace User Club letter in Poplar Computing Weekly Dec'83;");
aI("text=Jupiter Ace on the RUN;url=letters_PCN830901.html;title=Jupiter Ace on the RUN letter in PCN Sept'83;");
aI("text=What is FORTH-79;url=letters_PCWeekly830630.html;title=What is FORTH-79 letter in Poplar Computing Weekly June'83;");
aI("text=Codifying the Ace Cursor;url=letters_PCN830616p16.html;title=Codifying the Ace Cursor letter in PCNews June'83;");
aI("text=Ace Memory?;url=letters_YourComputer8305.html;title=Ace Memory? letter in Your Computer May'83;");
aI("text=Help With Forth Writing?;url=letters_YourComputer8301.html;title=Help With Forth Writing? letter in Your Computer Jan'83;");
}

 with(milonic=new menuname("doc_letters_82")){
orientation="vertical";
align="left";
top=420;
style=sub_2;
 aI("text=Ace and the Forth challenge;url=letters_PCWeekly821202.html;title=Ace and the Forth challenge letter in Poplar Computing Weekly Dec'82;");
aI("text=Abandoning BASIC for FORTH;url=letters_PCWeekly821209.html;title=Abandoning BASIC for FORTH letter in Poplar Computing Weekly Dec'82;");
aI("text=Ace Goes Back To School;url=letters_PCWeekly821111.html;title=Ace Goes Back To School letter in Poplar Computing Weekly Nov'82;");
 aI("text=Language Specification;url=letters_PCWeekly821018.html;title=Language Specification letter in Poplar Computing Weekly Nov'82;");
}




with(milonic=new menuname("doc_books")){
orientation="vertical";
align="left";
top=370;
style=sub_1;
aI("text=Jupiter ACE in Books;url=books.html;title=Jupiter ACE in Books;");
aI("text=Forth Books index;url=index_forth_books.html;title=Forth Books List;");
//aI("text=All Forth Books [WIP];url=index_forth_books.html;title=Forth Books List;");
 //aI("align=center;keepalive=0;showmenu=doc_adverts_magazine;text=Magazine Adverts;title=Magazine Adverts;");
}


 with(milonic=new menuname("doc_fortharticles")){
orientation="vertical";
align="left";
top=350;
style=sub_1;
aI("text=All Forth magazine articles;url=index_Forth_general.html;title=Forth articles form magazines;");
aI("align=center;keepalive=0;showmenu=doc_fortharticles_byte1980;text=Byte Magazine '80 [WIP];title=Byte Magazine 1980 Magazine ;");
aI("align=center;keepalive=0;showmenu=doc_fortharticles_goingforth;text=Going Forth;title=Going Forth series from Computing Today;");
aI("align=center;keepalive=0;showmenu=doc_fortharticles_goingforthagain;text=Going Forth Again;title=Going Forth Again series from Computing Today;");
aI("align=center;keepalive=0;showmenu=doc_fortharticles_MicroSystem;text=Micro System Magazine;title=Micro System Magazine;");
aI("text=Feature BASIC/FORTH;url=Forth_general_PCToday_8212_44.html;title=Feature BASIC/FORTH from Personal Computing Today '82;");
aI("text=PCW Forth Benchmarks;url=Forth_general_PCWorld_8301.html;title=PCW Forth Benchmarks;");
aI("text=IBM Comes FORTH;url=Forth_general_PCW-8307p198.html;title=IBM Comes FORTH from Personal Computer World '83;");
aI("text=Go Forth and Multiply;url=Forth_PCWGoForthAnd.html;title=Go Forth and Multiply from Personal Computer World '81.;");
aI("text=PCN GO FORTH : Part 1 ;url=Forth_general_PCN830408.html;title=PCN GO FORTH : Part 1;");
aI("text=Forth for Microcomputers;url=Forth_general_DrDobbs7805.html;title=Forth for Microcomputers from Dr Dobbs May'78.;");
aI("text=Factorials from '82;url=Forth_general_PC8209p151.html;title=Factorials from Practical Computing Sept '82.;");
}





////aI("align=center;keepalive=0;showmenu=doc_adverts_magazine;text=Magazine Adverts;title=Magazine Adverts;");
// doc_fortharticals
 //.

with(milonic=new menuname("doc_fortharticles_MicroSystem")){
orientation="vertical";
align="left";
top=400;
style=sub_2;
aI("text=Forth Intro;url=Forth_general-Micro-system-8303.html;title=Micro System Forth March'83;");
aI("text=AceForth Oct'83;url=tut_micro-sysystem-mag-8310.html;title=AceForth Oct'83;");
aI("text=AceForth Nov'83;url=tut_micro-sysystem-mag-8311.html;title=AceForth Nov 83;");
aI("text=AceForth Dec'83;url=tut_micro-sysystem-mag-8312.html;title=AceForth Dec 83;");
aI("text=AceForth Jan'84;url=tut_micro-sysystem-mag-8401.html;title=AceForth Jan 84;");
aI("text=AceForth Feb'84;url=tut_micro-sysystem-mag-8402.html;title=AceForth Feb 84;");
aI("text=AceForth May'84;url=tut_micro-sysystem-mag-8405.html;title=AceForth May 84;");
aI("text=AceForth Dec'84;url=tut_micro-sysystem-mag-8412.html;title=AceForth Dec 84;");

}

with(milonic=new menuname("doc_fortharticles_goingforthagain")){
orientation="vertical";
align="left";
top=420;
style=sub_2;
aI("text=Going Forth Again - part 1;url=Forth_general_ComputingToDay8305l.html;title=Going Forth Again - part 1;");
//aI("text=Going Forth Again - part 2;url=Forth_general_ComputingToDay8305l.html;title=Going Forth Again - part 2;");
//aI("text=Going Forth Again - part 3;url=Forth_general_ComputingToDay8305l.html;title=Going Forth Again - part 3;");
//aI("text=Going Forth Again - part 4;url=Forth_general_ComputingToDay8305l.html;title=Going Forth Again - part 4;");
}

with(milonic=new menuname("doc_fortharticles_goingforth")){
orientation="vertical";
align="left";
top=380;
style=sub_2;
aI("text=Going Forth - part 1;url=Forth_general_ComputingToday8201.html;title=Going Forth - part 1;");
aI("text=Going Forth - part 2;url=Forth_general_ComputingToday8202.html;title=Going Forth - part 2;");
aI("text=Going Forth - part 3;url=Forth_general_ComputingToday8203.html;title=Going Forth - part 3;");
aI("text=Going Forth - part 4;url=Forth_general_ComputingToday8204.html;title=Going Forth - part 4;");
}



with(milonic=new menuname("doc_fortharticles_byte1980")){
orientation="vertical";
align="left";
top=360;
style=sub_2;
aI("text=Editorial;url=Forth_general_byte8008_10.html;title=Byte 1980 Editorial;");
aI("text=What is FORTH?;url=Forth_general_byte8008_10.html;title=Byte 1980 What is FORTH?;");

//
//   WIP
//
}

// ########################################



// documents end ####################################################################

with(milonic=new menuname("faq")){
orientation="vertical";
align="left";
top=171;
style=submenuStyle;
aI("text=Manuals;url=usermanual.html;");
aI("text=Reviews & News, Letters;url=reviews.html;");
aI("text=Adverts;url=adverts.html;");
aI("text=Books;url=books.html;");
aI("text=Schematics;url=ace_hardware.html;");
aI("text=Ace Users;url=aceusers.html;");
aI("text=General Forth;url=index_Forth_general.html;");
aI("text=Videos;url=index_videos.html;");
}



// ########### programming menu options ################################################
with(milonic=new menuname("programming")){
orientation="vertical";
align="left";
top=171;
style=submenuStyle;

aI("text=ace-forth cross-compiler;url=prog_ace-forth-cross-compiler.html;");
aI("text=Program listings;url=index_program_listings.html;");
aI("text=ROM Disassembly English;url=http://www.jupiter-ace.co.uk/romlisting.html;");
aI("text=ROM Disassembly German;url=http://www.jupiter-ace.co.uk/romlisting_german.html;");
//
aI("text=Z80 m/code on the Ace;url=http://www.jupiter-ace.co.uk/prog_howtomakez80code.html;");
aI("text=Development Environment;url=http://www.jupiter-ace.co.uk/prog_ide-setup.html;");
aI("text=TAP files with TASM;url=http://www.jupiter-ace.co.uk/prog_tap_templates.html;");
aI("text=AutoRUN files with TASM ;url=http://www.jupiter-ace.co.uk/prog_autorun_templates.html;")
aI("text=Include files for TASM;url=http://www.jupiter-ace.co.uk/prog_includefiles.html;");
aI("text=Syntax Highlighting;url=http://www.jupiter-ace.co.uk/prog_TextSyntaxHighlighting.html;");
aI("text=Memory Map;url=http://www.jupiter-ace.co.uk/prog_memorymap.html;");
aI("text=Video screen layout;url=http://www.jupiter-ace.co.uk/doc_screenLayOut.html");
aI("text=IN key presses;url=http://www.jupiter-ace.co.uk/prog_keyboardread.html;");
//
//
aI("text=UDGs Designer;url=http://www.jupiter-ace.co.uk/sw_fonted_2.html;");
aI("text=WRD Forth Structure ;url=http://www.jupiter-ace.co.uk/sw_wrd_ricardo.html;");
aI("text=Manual Examples;url=http://www.jupiter-ace.co.uk/sw_AceManualCodeExamples.html;");

}


with(milonic=new menuname("emulators")){
orientation="vertical";
align="left";
top=171;
style=submenuStyle;
// aI("text=Java Emulator;url=zzace_09.html;");
aI("text=MS-Dos;url=ace32.html;");
aI("text=Windows;url=emulators_win.html;");
aI("text=Mac OS X;url=emulators_mac.html;");
aI("text=Linux;url=emulators_unix.html;");
aI("text=Sam Coupe;url=blaze.html;");
aI("text=Nintendo DS;url=emulators_DS.html;");
// aI("text=Emulator Utilities;url=EmulatorUtilities.html;");
aI("text=Retro Forth's;url=RetroForths.html;");
}

with(milonic=new menuname("links")){
top=171;
style=submenuStyle;
aI("text=Links Page Index !;url=links.html;title=Links Index Page;");
}



 // software subs ####################################################################################
//with(milonic=new menuname("sw_houses")){

//orientation="vertical";
//align="left";
//top=310; // 183
//style=sub_1;
//aI("text=SW House Index;url=http://www.jupiter-ace.co.uk/index_SWHouse_index.html;");
//aI("text=Boldfield Computing;url=http://www.jupiter-ace.co.uk/SWHouse_boldfield.html;");
//aI("text=Jupiter Cantab;url=http://www.jupiter-ace.co.uk/SWHouse_Cantab.html;");
//aI("text=Voyager Software;url=http://www.jupiter-ace.co.uk/SWHouse_voyagersoftware.html;");
//aI("text=Plasma Software;url=http://www.jupiter-ace.co.uk/SWHouse_JKennedy.html;");
//aI("text=R F Lopes;url=http://www.jupiter-ace.co.uk/SWHouse_RFLopes.html;");
//aI("text=Micro Marketing;url=http://www.jupiter-ace.co.uk/SWHouse_micromarketing.html;");
//aI("text=Hi-Tech Microsoft;url=http://www.jupiter-ace.co.uk/SWHouse_HiTechMicrosoft.html;");
//aI("text=Garry Knight;url=http://www.jupiter-ace.co.uk/SWHouse_GarryKnight.html;");
//aI("text=Remsoft;url=http://www.jupiter-ace.co.uk/SWHouse_Remsoft.html;");
//aI("text=D Leonard;url=http://www.jupiter-ace.co.uk/SWHouse_D_Leonard.html;");
//}




drawMenus();


