input metaobj;
filenametemplate "%j.mps";
beginfig(1);
setCurveDefaultOption("arrows","->");

newCircle.cat1(btex $\cdot$ etex) "circmargin(.2cm)";
newCircle.cat2(btex $\cdot$ etex) "circmargin(.2cm)";
newCircle.e1("") "circmargin(0cm)";
newCircle.e2("") "circmargin(0cm)";
newCircle.e3("") "circmargin(0cm)";
newHFan.st1(1cm, 0);
newHFan.st2(1cm, 0);
newHFan.st3(1cm, 0);
newTree.t5(e2)(st2);
newTree.t4(e1)(st1);
newTree.t3(cat1)(t4,t5);
newTree.t1(e3)(st3);
newTree.t0(cat2)(t3,t1);
t0.c=origin;
drawObj(t0);
endfig;
end
