typedef struct destruction_X_node_ { struct destruction_X_node_* next; void* cls; void (*destructor)(void*); } destruction_X_node;

static destruction_X_node* destruction_X_list = 0;

void cm_exit()
{
    while (destruction_X_list)
    {
        destruction_X_node* node = destruction_X_list;
        destruction_X_list = node->next;
        void* cls = node->cls;
        void (*destructor)(void*) = node->destructor;
        destructor(cls);
    }
}

void register_X_destructor(destruction_X_node* node)
{
    node->next = destruction_X_list;
    destruction_X_list = node;
}
