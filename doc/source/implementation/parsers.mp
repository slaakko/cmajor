input metaobj;
filenametemplate "%j.mps";
beginfig(1);
setCurveDefaultOption("arrows","->");

newCircle.a1(btex $a$ etex) "circmargin(.2cm)";
newBox.nt1(btex $nt(term:pt)$ etex);
newTree.t0(a1)(nt1);

newBox.plus(btex '+' etex) "dx(0.5cm)";
newCircle.cat1(btex $\cdot$ etex) "circmargin(.2cm)";
newTree.t1(cat1)(plus,t0);

newCircle.a2(btex $a$ etex) "circmargin(.2cm)";
newBox.nt2(btex $nt(term:mt)$ etex);
newTree.t2(a2)(nt2);

newBox.minus(btex '-' etex) "dx(0.5cm)";
newCircle.cat2(btex $\cdot$ etex) "circmargin(.2cm)";
newTree.t3(cat2)(minus,t2);

newCircle.alt1(btex $|$ etex) "circmargin(.2cm)";
newTree.t4(alt1)(t1,t3);

newCircle.closure(btex $*$ etex) "circmargin(.2cm)";
newTree.t5(closure)(t4);

newCircle.an(btex $a$ etex) "circmargin(.2cm)";
newBox.nt3(btex $nt(term:t)$ etex);
newTree.tn(an)(nt3);

newCircle.cat3(btex $\cdot$ etex) "circmargin(.2cm)";
newTree.t6(cat3)(tn,t5);

t6.c=origin;
drawObj(t6);
endfig;
end
