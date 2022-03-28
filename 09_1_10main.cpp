
#include <stdio.h>
#include "09_1_10task.cpp"

int main(void) {

    file_manager_t fm;
    setup_file_manager(&fm);
    create(1000);

    printf("dir => %d\n", create_dir("/B"));
    printf("dir => %d\n", create_dir("/D"));
    printf("dir => %d\n", create_dir("/C"));
    printf("dir => %d\n", create_dir("/A"));
    printf("dir => %d\n", create_dir("/A/.././A/folder_1"));
    printf("dir => %d\n", create_dir("/A/folder_2"));
    printf("dir => %d\n", create_dir("/A/folder_3"));
    change_dir("/A/folder_2");
    printf("file => %d\n", create_file("file_1", 200));
    printf("file => %d\n", create_file("/A/folder_2/file_2", 200));
    printf("file => %d\n", create_file("/A/folder_1/file_33", 500));
    printf("file => %d\n", create_file("/A/folder_3/file_55", 200));
    printf("file => %d\n", create_file(".././folder_2/file_3", 100));
    printf("file => %d\n", create_dir("./.."));

    //    remove("", 1);
    char full_path[128];
    get_cur_dir(full_path);
    //
    printf("full path => %s\n", full_path);
    //    Node *find = find_node(root, "/A/folder_2/../../A/./folder_2", NULL, 0);
    Node* find = find_node(root, "/A/folder_2", NULL, 0);
    //    remove("/A/folder_2", 1);
    printf("size => %d\n", size("/"));
    if (find != NULL) {
        printf("===============\n");
        for (int idx = 0; idx < find->children_num; idx += 1)
            printf("%s\n", find->children[idx]->name);
        printf("===============\n");
    }




    printf("===============\n");
    if (destroy())
        printf("root is %s\n", root == NULL ? "null" : "not null");
    printf("disk size => %d\n", fm_size);
    return 0;
}
