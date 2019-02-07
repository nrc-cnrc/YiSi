#!/usr/bin/perl -w

# This is a tokenizer for English based on the script by Yaser
# Al-Onaizan written for the 1999 JHU WS on SMT.

# the input file should be in text one sentence per line format. 
# Segments that are in between xml tags will not be tokenized.

use charnames ":full";
use utf8;

binmode(STDIN, ":encoding(utf8)");
binmode(STDOUT, ":encoding(utf-8-strict)");

while(<STDIN>){
    if ($_ !~ m/^\s*\<\/?[c\?sx]/){
	my $sent = $_;

	# substitute XML character entities
	$sent =~ s/&amp;/\&/g;
	$sent =~ s/&lt;/\</g;
	$sent =~ s/&gt;/\>/g;
	$sent =~ s/&apos;/\'/g;
	$sent =~ s/&quot;/\"/g;

	# store the pretokenized parts of the sentence delimited by XML tags
	my @pretokenized = ();
	while($sent =~ m/(\<.+?\>.+?\<\/.+?\>)/g){
	    push(@pretokenized,$1);
	}
	$sent =~ s/\<.+?\>.+?\<\/.+?\>/ PRETOKENIZED /g;

	my $output;
	
	$sent = &Estem_contractions($sent);
	$sent =~ s/ +/ /g;
	$sent = &Estem_elision($sent);
	$sent =~ s/ +/ /g;
	$sent = &Edelimit_tokens($sent);
	$sent =~ s/ +/ /g;
	$sent = &Emerge_abbr($sent);

	# put space before any period that is followed by the end of line.
	$sent =~ s/(\S+)\.(\s*)$/$1 \.$2/;
	$sent =~ tr/[ \t\r\n]//s; # supress multiple white spaces into single space
	$sent =~ s/^\s+//;

	# escape predetermined XML entities
	$sent =~ s/\&/&amp;/g;
	$sent =~ s/\</&lt;/g;
	$sent =~ s/\>/&gt;/g;
	$sent =~ s/\'/&apos;/g;
	$sent =~ s/\"/&quot;/g;

	if ( $sent eq "" ){
	    $output = "\n";
	}else{
	    my $isgarbage = 0;
	    my @chars = split(//,$sent);
	    for( my $j = 0; $j < @chars; $j++){
		if (ord($chars[$j]) > 128 ){
		    $isgarbage = 0;
		}
	    }

	    $sent =~ s/(\W+)([0-9]+)/$1 $2/g;
	    my @lexemes=split(/\s+/,$sent);

	    # put the pretokenized segments back in
	    for(my $i = 0; $i < @lexemes; $i++){
		if ($lexemes[$i] eq "PRETOKENIZED"){
		    $lexemes[$i] = shift(@pretokenized);
		}
	    }

	    die "Warning: pretokenized segments not re-inserted in the data after tokenization\n" if ($#pretokenized > -1);
	 
	    # print
	    if ($isgarbage == 0){
		print STDOUT join(" ",@lexemes),"\n";
	    }else{
		print STDOUT "CORRUPTED\n";
	    }
	}
    }else{
	print;
    }
}

sub Estem_contractions(){
    my($sent) = @_;
    $sent =~ s/\'t/ \'t/g;
    $sent =~ s/\'m/ \'m/g;
    $sent =~ s/\'re/ \'re/g;
    $sent =~ s/\'ll/ \'ll/g;
    $sent =~ s/\'ve/ \'ve/g;
    # put space before ambiguous contractions
    $sent =~ s/([^ ])\'s/$1 \'s/g;
    $sent =~ s/([^ ])\'d/$1 \'d/g;
    return $sent;
}

sub Estem_elision(){
    my($sent) = @_;
    # stems English elisions, except for the ambiguous cases of 's and 'd
    # $sent =~ s/Won \'t/Won* n\'t/g;
    # $sent =~ s/Can \'t/Can* n\'t/g;
    # $sent =~ s/Shan \'t/Shan* n\'t/g;
    # $sent =~ s/won \'t/won* n\'t/g;
    # $sent =~ s/can \'t/can* n\'t/g;
    # $sent =~ s/shan \'t/shan* n\'t/g;

    $sent =~ s/Won \'t/Will not/g;
    $sent =~ s/Can \'t/Can not/g;
    $sent =~ s/Shan \'t/Shall not/g;
    $sent =~ s/won \'t/will not/g;
    $sent =~ s/can \'t/can not/g;
    $sent =~ s/shan \'t/shall not/g;

    $sent =~ s/ \'m/ am/g;
    $sent =~ s/n \'t/ not/g;
    $sent =~ s/ \'re/ are/g;
    $sent =~ s/ \'ll/ will/g;
    $sent =~ s/ \'ve/ have/g;
    return $sent;
}

sub Edelimit_tokens(){
  my($sent) = @_;
  # puts spaces around punctuation and special symbols

  # stardardize quotes
  $sent =~ s/\'\' /\" /g;
  $sent =~ s/ \`\`/ \"/g;
  $sent =~ s/\'\'$/\"/g;
  $sent =~ s/^\`\`/\"/g;
    
  # put space after any period that's followed by a non-number and non-period
  $sent =~ s/\.([^0-9\.])/\. $1/g;

  # put space before any period that's followed by a space or another period, 
  # unless preceded byss another period
  # the following space is introduced in the previous command
  $sent =~ s/([^\.])\.([ \.])/$1 \.$2/g;
    
  # put space around sequences of colons and comas, unless they're
  # surrounded by numbers or other colons and comas
  $sent =~ s/([0-9:])\:([0-9:])/$1<CLTKN>$2/g;
  $sent =~ s/\:/ \: /g;
  $sent =~ s/([0-9]) ?<CLTKN> ?([0-9])/$1\:$2/g;
  $sent =~ s/([0-9,])\,([0-9,])/$1<CMTKN>$2/g;
  $sent =~ s/\,/ \, /g;
  $sent =~ s/([0-9]) ?<CMTKN> ?([0-9])/$1\,$2/g;

  # put space before any other punctuation and special symbol sequences
  $sent =~ s/([^ \!])(\!+)/$1 $2/g;
  $sent =~ s/([^ \?])(\?+)/$1 $2/g;
  $sent =~ s/([^ \;])(\;+)/$1 $2/g;
  $sent =~ s/([^ \"])(\"+)/$1 $2/g;
  $sent =~ s/([^ \)])(\)+)/$1 $2/g;
  $sent =~ s/([^ \(])(\(+)/$1 $2/g;
  $sent =~ s/([^ \/])(\/+)/$1 $2/g;
  $sent =~ s/([^ \&])(\&+)/$1 $2/g;
  $sent =~ s/([^ \^])(\^+)/$1 $2/g;
  $sent =~ s/([^ \%])(\%+)/$1 $2/g;
  $sent =~ s/([^ \$])(\$+)/$1 $2/g;
  $sent =~ s/([^ \+])(\++)/$1 $2/g;
  $sent =~ s/([^ \-])(\-+)/$1 $2/g;
  $sent =~ s/([^ \#])(\#+)/$1 $2/g;
  $sent =~ s/([^ \*])(\*+)/$1 $2/g;
  $sent =~ s/([^ \[])(\[+)/$1 $2/g;
  $sent =~ s/([^ \]])(\]+)/$1 $2/g;
  $sent =~ s/([^ \{])(\{+)/$1 $2/g;
  $sent =~ s/([^ \}])(\}+)/$1 $2/g;
  $sent =~ s/([^ \>])(\>+)/$1 $2/g;
  $sent =~ s/([^ \<])(\<+)/$1 $2/g;
  $sent =~ s/([^ \_])(\_+)/$1 $2/g;
  $sent =~ s/([^ \\])(\\+)/$1 $2/g;
  $sent =~ s/([^ \|])(\|+)/$1 $2/g;
  $sent =~ s/([^ \=])(\=+)/$1 $2/g;
  $sent =~ s/([^ \'])(\'+)/$1 $2/g;
  $sent =~ s/([^ \`])(\`+)/$1 $2/g;
  $sent =~ s/([^ \²])(\²+)/$1 $2/g;
  $sent =~ s/([^ \³])(\³+)/$1 $2/g;
  $sent =~ s/([^ \«])(\«+)/$1 $2/g;
  $sent =~ s/([^ \»])(\»+)/$1 $2/g;
  $sent =~ s/([^ \¢])(\¢+)/$1 $2/g;
  $sent =~ s/([^ \°])(\°+)/$1 $2/g;
    
  # put space after any other punctuation and special symbols sequences
  $sent =~ s/(\!+)([^ \!])/$1 $2/g;
  $sent =~ s/(\?+)([^ \?])/$1 $2/g;
  $sent =~ s/(\;+)([^ \;])/$1 $2/g;
  $sent =~ s/(\"+)([^ \"])/$1 $2/g;
  $sent =~ s/(\'+)([^ \'])/$1 $2/g;
  $sent =~ s/(\(+)([^ \(])/$1 $2/g;
  $sent =~ s/(\)+)([^ \)])/$1 $2/g;
  $sent =~ s/(\/+)([^ \/])/$1 $2/g;
  $sent =~ s/(\&+)([^ \&])/$1 $2/g;
  $sent =~ s/(\^+)([^ \^])/$1 $2/g;
  $sent =~ s/(\%+)([^ \%])/$1 $2/g;
  $sent =~ s/(\$+)([^ \$])/$1 $2/g;
  $sent =~ s/(\++)([^ \+])/$1 $2/g;
  $sent =~ s/(\-+)([^ \-])/$1 $2/g;
  $sent =~ s/(\#+)([^ \#])/$1 $2/g;
  $sent =~ s/(\*+)([^ \*])/$1 $2/g;
  $sent =~ s/(\[+)([^ \[])/$1 $2/g;
  $sent =~ s/(\]+)([^ \]])/$1 $2/g;
  $sent =~ s/(\}+)([^ \}])/$1 $2/g;
  $sent =~ s/(\{+)([^ \{])/$1 $2/g;
  $sent =~ s/(\\+)([^ \\])/$1 $2/g;
  $sent =~ s/(\|+)([^ \|])/$1 $2/g;
  $sent =~ s/(\_+)([^ \_])/$1 $2/g;
  $sent =~ s/(\<+)([^ \<])/$1 $2/g;
  $sent =~ s/(\>+)([^ \>])/$1 $2/g;
  $sent =~ s/(\=+)([^ \=])/$1 $2/g;
  $sent =~ s/(\`+)([^ \`])/$1 $2/g;
  # $sent =~ s/(\'+)([^ \'])/$1 $2/g;      # do not insert space after forward tic
  $sent =~ s/(\²+)([^ \²])/$1 $2/g;
  $sent =~ s/(\³+)([^ \³])/$1 $2/g;
  $sent =~ s/(\«+)([^ \«])/$1 $2/g;
  $sent =~ s/(\»+)([^ \»])/$1 $2/g;
  $sent =~ s/(\¢+)([^ \¢])/$1 $2/g;
  $sent =~ s/(\°+)([^ \°])/$1 $2/g;

  # separate alphabetical tokens
  $sent =~ s/([\p{Alphabetic}\p{InLatin1Supplement}]+)/ $1 /g;
  $sent =~ s/ \' +s / \'s /g;
  $sent =~ s/ \' +d / \'d /g;

  # group back together xml entities
  $sent =~ s/(\&)\s+([a-z]+)\s+(\;)/ $1$2$3 /g;

  return $sent;
}


sub Emerge_abbr(){
  my($sent) = @_;
  $sent =~ s/U\s+\.\s+S\s+\.\s+S\s+\.\s+R\s+\./U.S.S.R./g;
  $sent =~ s/U\s+\.\s+S\s+\.\s+A\s+\./U.S.A./g;
  $sent =~ s/P\s+\.\s+E\s+\.\s+I\s+\./P.E.I./g;
  $sent =~ s/p\s+\.\s+m\s+\./p.m./g;
  $sent =~ s/a\s+\.\s+m\s+\./a.m./g;
  $sent =~ s/U\s+\.\s+S\s+\./U.S./g;
  $sent =~ s/B\s+\.\s+C\s+\./B.C./g;
  $sent =~ s/vol\s+\./vol./g;
  $sent =~ s/viz\s+\./viz./g;
  $sent =~ s/v\s+\./v./g;
  $sent =~ s/terr\s+\./terr./g;
  $sent =~ s/tel\s+\./tel./g;
  $sent =~ s/subss\s+\./subss./g;
  $sent =~ s/subs\s+\./subs./g;
  $sent =~ s/sub\s+\./sub./g;
  $sent =~ s/sess\s+\./sess./g;
  $sent =~ s/seq\s+\./seq./g;
  $sent =~ s/sec\s+\./sec./g;
  $sent =~ s/rév\s+\./rév./g;
  $sent =~ s/rev\s+\./rev./g;
  $sent =~ s/repl\s+\./repl./g;
  $sent =~ s/rep\s+\./rep./g;
  $sent =~ s/rel\s+\./rel./g;
  $sent =~ s/paras\s+\./paras./g;
  $sent =~ s/para\s+\./para./g;
  $sent =~ s/op\s+\./op./g;
  $sent =~ s/nom\s+\./nom./g;
  $sent =~ s/nil\s+\./nil./g;
  $sent =~ s/mr\s+\./mr./g;
  $sent =~ s/lég\s+\./lég./g;
  $sent =~ s/loc\s+\./loc./g;
  $sent =~ s/jur\s+\./jur./g;
  $sent =~ s/int\s+\./int./g;
  $sent =~ s/incl\s+\./incl./g;
  $sent =~ s/inc\s+\./inc./g;
  $sent =~ s/id\s+\./id./g;
  $sent =~ s/ibid\s+\./ibid./g;
  $sent =~ s/hum\s+\./hum./g;
  $sent =~ s/hon\s+\./hon./g;
  $sent =~ s/gén\s+\./gén./g;
  $sent =~ s/etc\s+\./etc./g;
  $sent =~ s/esp\s+\./esp./g;
  $sent =~ s/eg\s+\./eg./g;
  $sent =~ s/eds\s+\./eds./g;
  $sent =~ s/ed\s+\./ed./g;
  $sent =~ s/crit\s+\./crit./g;
  $sent =~ s/corp\s+\./corp./g;
  $sent =~ s/conf\s+\./conf./g;
  $sent =~ s/comp\s+\./comp./g;
  $sent =~ s/comm\s+\./comm./g;
  $sent =~ s/com\s+\./com./g;
  $sent =~ s/co\s+\./co./g;
  $sent =~ s/civ\s+\./civ./g;
  $sent =~ s/cit\s+\./cit./g;
  $sent =~ s/chap\s+\./chap./g;
  $sent =~ s/cert\s+\./cert./g;
  $sent =~ s/ass\s+\./ass./g;
  $sent =~ s/arts\s+\./arts./g;
  $sent =~ s/art\s+\./art./g;
  $sent =~ s/alta\s+\./alta./g;
  $sent =~ s/al\s+\./al./g;
  $sent =~ s/Yes\s+\./Yes./g;
  $sent =~ s/XX\s+\./XX./g;
  $sent =~ s/XVIII\s+\./XVIII./g;
  $sent =~ s/XVII\s+\./XVII./g;
  $sent =~ s/XVI\s+\./XVI./g;
  $sent =~ s/XV\s+\./XV./g;
  $sent =~ s/XIX\s+\./XIX./g;
  $sent =~ s/XIV\s+\./XIV./g;
  $sent =~ s/XIII\s+\./XIII./g;
  $sent =~ s/XII\s+\./XII./g;
  $sent =~ s/XI\s+\./XI./g;
  $sent =~ s/X\s+\./X./g;
  $sent =~ s/Wash\s+\./Wash./g;
  $sent =~ s/Vol\s+\./Vol./g;
  $sent =~ s/Vict\s+\./Vict./g;
  $sent =~ s/Ves\s+\./Ves./g;
  $sent =~ s/Va\s+\./Va./g;
  $sent =~ s/VIII\s+\./VIII./g;
  $sent =~ s/VII\s+\./VII./g;
  $sent =~ s/VI\s+\./VI./g;
  $sent =~ s/V\s+\./V./g;
  $sent =~ s/Univ\s+\./Univ./g;
  $sent =~ s/Trib\s+\./Trib./g;
  $sent =~ s/Tr\s+\./Tr./g;
  $sent =~ s/Tex\s+\./Tex./g;
  $sent =~ s/Surr\s+\./Surr./g;
  $sent =~ s/Supp\s+\./Supp./g;
  $sent =~ s/Sup\s+\./Sup./g;
  $sent =~ s/Stud\s+\./Stud./g;
  $sent =~ s/Ste\s+\./Ste./g;
  $sent =~ s/Stat\s+\./Stat./g;
  $sent =~ s/Stan\s+\./Stan./g;
  $sent =~ s/St\s+\./St./g;
  $sent =~ s/Soc\s+\./Soc./g;
  $sent =~ s/Sgt\s+\./Sgt./g;
  $sent =~ s/Sess\s+\./Sess./g;
  $sent =~ s/Sept\s+\./Sept./g;
  $sent =~ s/Sch\s+\./Sch./g;
  $sent =~ s/Sask\s+\./Sask./g;
  $sent =~ s/ST\s+\./ST./g;
  $sent =~ s/Ry\s+\./Ry./g;
  $sent =~ s/Rev\s+\./Rev./g;
  $sent =~ s/Rep\s+\./Rep./g;
  $sent =~ s/Reg\s+\./Reg./g;
  $sent =~ s/Ref\s+\./Ref./g;
  $sent =~ s/Qué\s+\./Qué./g;
  $sent =~ s/Que\s+\./Que./g;
  $sent =~ s/Pub\s+\./Pub./g;
  $sent =~ s/Pty\s+\./Pty./g;
  $sent =~ s/Prov\s+\./Prov./g;
  $sent =~ s/Prop\s+\./Prop./g;
  $sent =~ s/Prof\s+\./Prof./g;
  $sent =~ s/Probs\s+\./Probs./g;
  $sent =~ s/Plc\s+\./Plc./g;
  $sent =~ s/Pas\s+\./Pas./g;
  $sent =~ s/Parl\s+\./Parl./g;
  $sent =~ s/Pa\s+\./Pa./g;
  $sent =~ s/Oxf\s+\./Oxf./g;
  $sent =~ s/Ont\s+\./Ont./g;
  $sent =~ s/Okla\s+\./Okla./g;
  $sent =~ s/Nw\s+\./Nw./g;
  $sent =~ s/Nos\s+\./Nos./g;
  $sent =~ s/No\s+\./No./g;
  $sent =~ s/Nfld\s+\./Nfld./g;
  $sent =~ s/NOC\s+\./NOC./g;
  $sent =~ s/Mut\s+\./Mut./g;
  $sent =~ s/Mtl\s+\./Mtl./g;
  $sent =~ s/Ms\s+\./Ms./g;
  $sent =~ s/Mrs\s+\./Mrs./g;
  $sent =~ s/Mr\s+\./Mr./g;
  $sent =~ s/Mod\s+\./Mod./g;
  $sent =~ s/Minn\s+\./Minn./g;
  $sent =~ s/Mich\s+\./Mich./g;
  $sent =~ s/Mgr\s+\./Mgr./g;
  $sent =~ s/Mfg\s+\./Mfg./g;
  $sent =~ s/Messrs\s+\./Messrs./g;
  $sent =~ s/Mass\s+\./Mass./g;
  $sent =~ s/Mar\s+\./Mar./g;
  $sent =~ s/Man\s+\./Man./g;
  $sent =~ s/Maj\s+\./Maj./g;
  $sent =~ s/MURRAY\s+\./MURRAY./g;
  $sent =~ s/MR\s+\./MR./g;
  $sent =~ s/M\s+\./M./g;
  $sent =~ s/Ltd\s+\./Ltd./g;
  $sent =~ s/Ll\s+\./Ll./g;
  $sent =~ s/Ld\s+\./Ld./g;
  $sent =~ s/LTD\s+\./LTD./g;
  $sent =~ s/Jun\s+\./Jun./g;
  $sent =~ s/Jr\s+\./Jr./g;
  $sent =~ s/JJ\s+\./JJ./g;
  $sent =~ s/JA\s+\./JA./g;
  $sent =~ s/Ir\s+\./Ir./g;
  $sent =~ s/Int\s+\./Int./g;
  $sent =~ s/Inst\s+\./Inst./g;
  $sent =~ s/Ins\s+\./Ins./g;
  $sent =~ s/Inc\s+\./Inc./g;
  $sent =~ s/Imm\s+\./Imm./g;
  $sent =~ s/Ill\s+\./Ill./g;
  $sent =~ s/IX\s+\./IX./g;
  $sent =~ s/IV\s+\./IV./g;
  $sent =~ s/INC\s+\./INC./g;
  $sent =~ s/III\s+\./III./g;
  $sent =~ s/II\s+\./II./g;
  $sent =~ s/I\s+\./I./g;
  $sent =~ s/Hum\s+\./Hum./g;
  $sent =~ s/Hon\s+\./Hon./g;
  $sent =~ s/Harv\s+\./Harv./g;
  $sent =~ s/Hagg\s+\./Hagg./g;
  $sent =~ s/HON\s+\./HON./g;
  $sent =~ s/Geo\s+\./Geo./g;
  $sent =~ s/Genl\s+\./Genl./g;
  $sent =~ s/Gen\s+\./Gen./g;
  $sent =~ s/Gaz\s+\./Gaz./g;
  $sent =~ s/Fin\s+\./Fin./g;
  $sent =~ s/Fed\s+\./Fed./g;
  $sent =~ s/Feb\s+\./Feb./g;
  $sent =~ s/Fam\s+\./Fam./g;
  $sent =~ s/Fac\s+\./Fac./g;
  $sent =~ s/Europ\s+\./Europ./g;
  $sent =~ s/Eur\s+\./Eur./g;
  $sent =~ s/Esq\s+\./Esq./g;
  $sent =~ s/Enr\s+\./Enr./g;
  $sent =~ s/Eng\s+\./Eng./g;
  $sent =~ s/Eliz\s+\./Eliz./g;
  $sent =~ s/Edw\s+\./Edw./g;
  $sent =~ s/Educ\s+\./Educ./g;
  $sent =~ s/Dr\s+\./Dr./g;
  $sent =~ s/Doc\s+\./Doc./g;
  $sent =~ s/Dist\s+\./Dist./g;
  $sent =~ s/Dept\s+\./Dept./g;
  $sent =~ s/Dears\s+\./Dears./g;
  $sent =~ s/Dal\s+\./Dal./g;
  $sent =~ s/Ct\s+\./Ct./g;
  $sent =~ s/Cst\s+\./Cst./g;
  $sent =~ s/Crim\s+\./Crim./g;
  $sent =~ s/Cr\s+\./Cr./g;
  $sent =~ s/Cowp\s+\./Cowp./g;
  $sent =~ s/Corp\s+\./Corp./g;
  $sent =~ s/Conv\s+\./Conv./g;
  $sent =~ s/Cons\s+\./Cons./g;
  $sent =~ s/Conn\s+\./Conn./g;
  $sent =~ s/Comp\s+\./Comp./g;
  $sent =~ s/Comm\s+\./Comm./g;
  $sent =~ s/Com\s+\./Com./g;
  $sent =~ s/Colum\s+\./Colum./g;
  $sent =~ s/Co\s+\./Co./g;
  $sent =~ s/Cl\s+\./Cl./g;
  $sent =~ s/Civ\s+\./Civ./g;
  $sent =~ s/Cir\s+\./Cir./g;
  $sent =~ s/Chas\s+\./Chas./g;
  $sent =~ s/Ch\s+\./Ch./g;
  $sent =~ s/Cf\s+\./Cf./g;
  $sent =~ s/Cdn\s+\./Cdn./g;
  $sent =~ s/Cass\s+\./Cass./g;
  $sent =~ s/Cas\s+\./Cas./g;
  $sent =~ s/Car\s+\./Car./g;
  $sent =~ s/Can\s+\./Can./g;
  $sent =~ s/Calif\s+\./Calif./g;
  $sent =~ s/Cal\s+\./Cal./g;
  $sent =~ s/Bros\s+\./Bros./g;
  $sent =~ s/Bl\s+\./Bl./g;
  $sent =~ s/Bd\s+\./Bd./g;
  $sent =~ s/Aust\s+\./Aust./g;
  $sent =~ s/Aug\s+\./Aug./g;
  $sent =~ s/Assur\s+\./Assur./g;
  $sent =~ s/Assn\s+\./Assn./g;
  $sent =~ s/App\s+\./App./g;
  $sent =~ s/Am\s+\./Am./g;
  $sent =~ s/Alta\s+\./Alta./g;
  $sent =~ s/Admin\s+\./Admin./g;
  $sent =~ s/Adjut\s+\./Adjut./g;
  $sent =~ s/APPLIC\s+\./APPLIC./g;
  
  return $sent;

}

