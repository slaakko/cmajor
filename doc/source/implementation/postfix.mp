input metaobj;
filenametemplate "%j.mps";
beginfig(1);
setCurveDefaultOption("arrows","->");

newCircle.closure(btex $*$ etex) "circmargin(.2cm)";
newCircle.e1("") "circmargin(0cm)";
newHFan.st1(1cm, 0);
newTree.t1(e1)(st1);
newTree.t0(closure)(t1);

newCircle.positive(btex $+$ etex) "circmargin(.2cm)";
newCircle.e2("") "circmargin(0cm)";
newHFan.st2(1cm, 0);
newTree.t2(e2)(st2);
newTree.t3(positive)(t2);

newCircle.optional(btex $?$ etex) "circmargin(.2cm)";
newCircle.e3("") "circmargin(0cm)";
newHFan.st3(1cm, 0);
newTree.t4(e3)(st3);
newTree.t5(optional)(t4);

t0.c=origin;
t3.c=origin+(2cm,0);
t5.c=origin+(4cm,0)

drawObj(t0);
drawObj(t3);
drawObj(t5);
endfig;
end
