%destruction$node = type { %destruction$node*, i8*, void (i8*)* }

@destruction$list = global %destruction$node* null

define void @cm_exit()
{
    %node_ptr = alloca %destruction$node*
    %1 = load %destruction$node*, %destruction$node** @destruction$list
    store %destruction$node* %1, %destruction$node** %node_ptr
    br label %loop

loop:
    %2 = load %destruction$node*, %destruction$node** %node_ptr
    %3 = icmp ne %destruction$node* %2, null
    br i1 %3, label %next, label %end

next:
    %4 = getelementptr %destruction$node, %destruction$node* %2, i32 0, i32 0
    %5 = load %destruction$node*, %destruction$node** %4
    store %destruction$node* %5, %destruction$node** %node_ptr
    %6 = getelementptr %destruction$node, %destruction$node* %2, i32 0, i32 1
    %7 = load i8*, i8** %6
    %8 = getelementptr %destruction$node, %destruction$node* %2, i32 0, i32 2
    %9 = load void (i8*)*, void (i8*)** %8
    call void %9(i8* %7)
    br label %loop

end:
    ret void
}

define void @register$destructor(%destruction$node* %node)
{
    %1 = load %destruction$node*, %destruction$node** @destruction$list
    %2 = getelementptr %destruction$node, %destruction$node* %node, i32 0, i32 0
    store %destruction$node* %1, %destruction$node** %2
    store %destruction$node* %node, %destruction$node** @destruction$list
    ret void
}
