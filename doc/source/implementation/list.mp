input metaobj;
filenametemplate "%j.mps";
beginfig(1);
setCurveDefaultOption("arrows","->");

newHFan.st1(1cm, 0);
newCircle.e1("") "circmargin(0cm)";
newCircle.cat1(btex $\cdot$ etex) "circmargin(.2cm)";
newCircle.list(btex \% etex) "circmargin(.2cm)";

newCircle.e4("") "circmargin(0cm)";
newHFan.st5(1cm, 0);
newHFan.st3(1cm, 0);
newCircle.e3("") "circmargin(0cm)";
newCircle.cat2(btex $\cdot$ etex) "circmargin(.2cm)";
newTree.t7(e4)(st5);
newTree.t6(e3)(st3);
newTree.t5(cat2)(t6,t7);

newCircle.closure(btex $*$ etex) "circmargin(.2cm)";
newCircle.e2("") "circmargin(0cm)";
newHFan.st2(1cm, 0);
newTree.t3(e2)(st2);
newTree.t4(closure)(t5);

newTree.t2(e1)(st1);
newTree.t1(cat1)(t2,t4);
newTree.t0(list)(t1);
t0.c=origin;
drawObj(t0);
endfig;
end
