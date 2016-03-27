input metaobj;
filenametemplate "%j.mps";
beginfig(1);
setCurveDefaultOption("arrows","->");

newCircle.xor(btex $\hat{}$ etex) "circmargin(.2cm)";
newCircle.e1("") "circmargin(0cm)";
newCircle.e2("") "circmargin(0cm)";
newHFan.st1(1cm, 0);
newHFan.st2(1cm, 0);
newTree.t2(e2)(st2);
newTree.t1(e1)(st1);
newTree.t0(xor)(t1,t2);
t0.c=origin;
drawObj(t0);
endfig;
end
