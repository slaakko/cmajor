cmunit -backend=llvm -config=debug tests.cms
cmunit -backend=llvm -config=release tests.cms
cmunit -backend=c -config=debug tests.cms
cmunit -backend=c -config=release tests.cms
