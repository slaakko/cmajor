input metaobj;
filenametemplate "%j.mps";
beginfig(1);
newBox.DefaultConstructible(btex System.Concepts.DefaultConstructible$<$T$>$ etex);
newBox.CopyConstructible(btex System.Concepts.CopyConstructible$<$T$>$ etex);
newBox.Destructible(btex System.Concepts.Destructible$<$T$>$ etex);
newBox.Assignable(btex System.Concepts.Assignable$<$T$>$ etex);
newBox.Assignable2(btex System.Concepts.Assignable$<$T, U$>$ etex);
newBox.Semiregular(btex System.Concepts.Semiregular$<$T$>$ etex);
newBox.EqualityComparable(btex System.Concepts.EqualityComparable$<$ etex);
newBox.EqualityComparable2(btex System.Concepts.EqualityComparable$<$T, U$>$ etex);
newBox.LessThanComparable(btex System.Concepts.LessThanComparable$<$T$>$ etex);
newBox.LessThanComparable2(btex System.Concepts.LessThanComparable$<$T, U$>$ etex);
newBox.Regular(btex System.Concepts.Regular$<$T$>$ etex);
newBox.TotallyOrdered(btex System.Concepts.TotallyOrdered$<$T$>$ etex);
newBox.TotallyOrdered2(btex System.Concepts.TotallyOrdered$<$T, U$>$ etex);
newBox.TrivialIterator(btex System.Concepts.TrivialIterator$<$T$>$ etex);
newBox.OutputIterator(btex System.Concepts.OutputIterator$<$T$>$ etex);
newBox.InputIterator(btex System.Concepts.InputIterator$<$T$>$ etex);
newBox.ForwardIterator(btex System.Concepts.ForwardIterator$<$T$>$ etex);
newBox.BidirectionalIterator(btex System.Concepts.BidirectionalIterator$<$T$>$ etex);
newBox.RandomAccessIterator(btex System.Concepts.RandomAccessIterator$<$T$>$ etex);
newBox.UnaryFunction(btex System.Concepts.UnaryFunction$<$T$>$ etex);
newBox.BinaryFunction(btex System.Concepts.BinaryFunction$<$T$>$ etex);
newBox.UnaryPredicate(btex System.Concepts.UnaryPredicate$<$T$>$ etex);
newBox.BinaryPredicate(btex System.Concepts.BinaryPredicate$<$T$>$ etex);
newBox.Relation(btex System.Concepts.Relation$<$T$>$ etex);
newBox.Relation2(btex System.Concepts.Relation$<$T, U, V$>$ etex);
newBox.UnaryOperation(btex System.Concepts.UnaryOperation$<$T$>$ etex);
newBox.BinaryOperation(btex System.Concepts.BinaryOperation$<$T$>$ etex);
newBox.HashFunction(btex System.Concepts.HashFunction$<$T, Key$>$ etex);
newBox.KeySelectionFunction(btex System.Concepts.KeySelectionFunction$<$T, Key, Value$>$ etex);
newBox.Container(btex System.Concepts.Container$<$T$>$ etex);
newBox.ForwardContainer(btex Sytem.Concepts.ForwardContainer$<$T$>$ etex);
newBox.FrontInsertionSequence(btex System.Concepts.FrontInsertionSequence$<$T$>$ etex);
newBox.BackInsertionSequence(btex System.Concepts.BackInsertionSequence$<$T$>$ etex);
newBox.InsertionSequence(btex System.Concepts.InsertionSequence$<$T$>$ etex);
newBox.BidirectionalContainer(btex System.Concepts.BidirectionalContainer$<$T$>$ etex);
newBox.RandomAccessContainer(btex System.Concepts.RandomAccessContainer$<$T$>$ etex);
newBox.Integer(btex System.Concepts.Integer$<$T$>$ etex);
newBox.SignedInteger(btex System.Concepts.SignedInteger$<$T$>$ etex);
newBox.UnsignedInteger(btex System.Concepts.UnsignedInteger$<$T$>$ etex);
newBox.AdditiveSemigroup(btex System.Concepts.AdditiveSemigroup$<$T$>$ etex);
newBox.MultiplicativeSemigroup(btex System.Concepts.MultiplicativeSemigroup$<$T$>$ etex);
newBox.OrderedAdditiveSemigroup(btex System.Concepts.OrderedAdditiveSemigroup$<$T$>$ etex);
newBox.OrderedMultiplicativeSemigroup(btex System.Concepts.OrderedMultiplicativeSemigroup$<$T$>$ etex);
newBox.AdditiveMonoid(btex System.Concepts.AdditiveMonoid$<$T$>$ etex);
newBox.MultiplicativeMonoid(btex System.Concepts.MultiplicativeMonoid$<$T$>$ etex);
newBox.AdditiveGroup(btex System.Concepts.AdditiveGroup$<$T$>$ etex);
newBox.MultiplicativeGroup(btex System.Concepts.MultiplicativeGroup$<$T$>$ etex);
newBox.OrderedAdditiveMonoid(btex System.Concepts.OrderedAdditiveMonoid$<$T$>$ etex);
newBox.OrderedAdditiveGroup(btex System.Concepts.OrderedAdditiveGroup$<$T$>$ etex);
newBox.Semiring(btex System.Concepts.Semiring$<$T$>$ etex);
newBox.CommutativeSemiring(btex System.Concepts.CommutativeSemiring$<$T$>$ etex);
newBox.EuclideanSemiring(btex System.Concepts.EuclideanSemiring$<$T$>$ etex);
newBox.Common(btex System.Concepts.Common$<$T, U$>$ etex);
newTree.BidI(BidirectionalIterator)(RandomAccessIterator);
newTree.ForI(ForwardIterator)(BidI);
newTree.Inp(InputIterator)(ForI);
newTree.Triv(TrivialIterator)(OutputIterator,Inp);
newTree.BDC(BidirectionalContainer)(RandomAccessContainer);
newTree.ForCont(ForwardContainer)(InsertionSequence,FrontInsertionSequence,BDC,BackInsertionSequence);
newTree.Cont(Container)(ForCont);
newHBox.b(Triv);
scaleObj(b, 0.75);
b.c = origin;
drawObj(b);
endfig;
end

