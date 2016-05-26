input metaobj;
filenametemplate "%j.mps";
beginfig(1);
setCurveDefaultOption("arrows","<-");

newBox.root(btex Root etex) "dx(0.5cm)";

newBox.some1(btex SomeObject1 etex) "dx(0.5cm)";

newBox.some2(btex SomeObject2 etex) "dx(0.5cm)";

newBox.cont(btex Container etex) "dx(0.5cm)";

newTree.t1(root)(some1,some2,cont);

newBox.visitor(btex Visitor etex) "dx(0.5cm)";

newBox.algo1(btex Algorithm1 etex) "dx(0.5cm)";

newBox.algo2(btex Algorithm2 etex) "dx(0.5cm)";

newTree.t2(visitor)(algo1,algo2);

t1.c=origin;
t1.c-t2.c=(0,3cm);
drawObj(t1,t2);

endfig;
end
