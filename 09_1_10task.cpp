
#include "os_file.h"
#include "string.h"

typedef struct Node {
    int size;
    char name[33];
    Node* parent;
    Node** children;
    int children_num;
} Node;

unsigned int fm_size = 0;
Node* root = NULL;
Node* current_dir = NULL;

// ===== Node Function =====
Node* create_node(
    char const* name,
    Node* parent,
    Node** children,
    int        size,
    int        children_num);
int add_node(Node** node, char const* name, int size);
int find_child(Node* node, char const* name);
Node* find_node(Node* node, char const* path, char* dest, int add);
int  delete_child(Node** node, char const* name);
void delete_node(Node** node);


// ===== other function =====
int get_path_array(char const* path, char*** strings, int* size);
int delete_path_array(char*** strings, int size);
int recursive_size(Node* node);
void full_path(Node* node, char** bfs);

// ===== file_manager function =====
int create(int disk_size);
int destroy();
int create_dir(char const* path);
int create_file(char const* path, int file_size);
int remove(char const* path, int recursive);
int change_dir(char const* path);
void get_cur_dir(char* dst);
int size(char const* path);


void setup_file_manager(file_manager_t* fm) {
    fm->create = create;
    fm->destroy = destroy;
    fm->create_dir = create_dir;
    fm->create_file = create_file;
    fm->remove = remove;
    fm->change_dir = change_dir;
    fm->get_cur_dir = get_cur_dir;
    fm->size = size;
}





// ===== Node function =====
Node* create_node(
    const char* name,
    Node* parent,
    Node** children,
    int size,
    int children_num)
{
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
        return NULL;

    strcpy(new_node->name, name);

    new_node->parent = parent;
    new_node->children = children;
    new_node->size = size;
    new_node->children_num = children_num;

    return new_node;
}
int add_node(Node** node, char const* name, int size)
{
    if (*node == NULL)
        return 0;

    if ((*node)->children_num == 0)
        (*node)->children = (Node**)malloc(sizeof(Node*));
    else
        (*node)->children = (Node**)realloc((*node)->children, sizeof(Node*) * ((*node)->children_num + 1));

    if ((*node)->children == NULL)
        return 0;


    (*node)->children[(*node)->children_num] = create_node(name, *node, NULL, size, 0);
    if ((*node)->children[(*node)->children_num] == NULL)
        return 0;
    (*node)->children_num += 1;
    return 1;
}
int find_child(Node* node, char const* name) {
    if (node == NULL || node->children_num == 0)
        return -1;

    for (int index = 0; index < node->children_num; index += 1)
        if (strcmp(node->children[index]->name, name) == 0)
            return index;
    return -1;
}
Node* find_node(Node* node, char const* path, char* dest, int add)
{
    if (node == NULL)
        return NULL;

    Node* current = node;
    char** array;
    int array_size;

    get_path_array(path, &array, &array_size);
    if (dest != NULL)
        strcpy(dest, (array_size == 0 ? ".." : array[array_size - 1]));

    for (int idx = 0; idx < array_size - add; idx += 1) {
        if (strcmp(array[idx], "..") == 0) {
            if (current->parent == NULL) {
                current = NULL;
                break;
            }

            current = current->parent;
        }
        else if (strcmp(array[idx], ".") == 0) {
            continue;
        }
        else {
            int index = find_child(current, array[idx]);

            if (index == -1) {
                current = NULL;
                break;
            }

            current = current->children[index];
        }
    }
    delete_path_array(&array, array_size);
    return current;
}
int delete_child(Node** node, char const* name) {
    int index = find_child(*node, name);

    if (index == -1)
        return 0;

    delete_node(&(*node)->children[index]);

    for (int idx = index; idx < (*node)->children_num - 1; idx += 1)
        (*node)->children[idx] = (*node)->children[idx + 1];


    (*node)->children[(*node)->children_num - 1] = NULL;
    (*node)->children_num -= 1;
    return 1;
}
void delete_node(Node** node)
{
    if (*node == NULL)
        return;

    for (int idx = 0; idx < (*node)->children_num; idx += 1) {
        delete_node(&(*node)->children[idx]);
    }

    free((*node)->children);
    (*node)->children = NULL;
    (*node)->children_num = 0;

    free(*node);
    *node = NULL;
}
// ===== other function =====
int get_path_array(char const* path, char*** strings, int* size)
{
    *strings = (char**)malloc(sizeof(char*));

    if (*strings == NULL)
        return 0;

    *size = 0;
    char name[33];
    int  idx_name = 0;

    for (int idx = 0; idx < strlen(path); idx += 1) {
        if (path[idx] != '/') {
            name[idx_name++] = path[idx];
            if (idx + 1 == strlen(path) || path[idx + 1] == '/') {
                name[idx_name] = '\0';
                if (*size != 0)
                    *strings = (char**)realloc(*strings, sizeof(char*) * (*size + 1));

                if (*strings == NULL)
                    return 0;

                (*strings)[*size] = (char*)malloc(sizeof(char) * strlen(name) + 1);

                if ((*strings)[*size] == NULL)
                    return 0;

                strcpy((*strings)[*size], name);
                *size += 1;
                idx_name = 0;
            }
        }
    }

    return 1;
}
int delete_path_array(char*** strings, int size)
{
    if (size == 0 || *strings == NULL)
        return 0;

    for (int idx = 0; idx < size; idx += 1) {
        free((*strings)[idx]);
        (*strings)[idx] = NULL;
    }

    free(*strings);
    *strings = NULL;
    return 1;
}
int recursive_size(Node* node) {
    if (node == NULL)
        return 0;

    int result = 0;

    for (int idx = 0; idx < node->children_num; idx += 1)
        result += recursive_size(node->children[idx]);

    result += node->size;
    return result;
}
void full_path(Node* node, char** bfs) {
    if (node == NULL)
        return;

    full_path(node->parent, bfs);

    strcpy(&(*bfs)[strlen(*bfs)], node->name);
    if (node->parent != NULL)
        strcat(*bfs, "/");
}
// ===== file_manager function =====
int create(int disk_size)
{
    if (root != NULL)
        return 0;

    current_dir = root = create_node("/", NULL, NULL, 0, 0);
    if (root == NULL)
        return 0;

    fm_size = disk_size;
    return 1;
}
int destroy()
{
    if (root == NULL)
        return 0;

    delete_node(&root);
    fm_size = 0;
    return 1;
}
int create_dir(char const* path)
{
    return create_file(path, 0);
}
int create_file(char const* path, int file_size)
{
    if (root == NULL || strlen(path) > 128 || file_size > fm_size)
        return 0;

    char name[33];
    Node* find = find_node(path[0] == '/' ? root : current_dir, path, name, 1);

    if (find == NULL || name[0] == '.' || find->size != 0)
        return 0;

    int result = add_node(&find, name, file_size);
    if (result)
        fm_size -= file_size;

    return result;
}
int remove(char const* path, int recursive)
{
    if (root == NULL)
        return 0;

    Node* find = find_node(path[0] == '/' ? root : current_dir, path, NULL, 0);
    if (find == NULL || find->parent == NULL)
        return 0;

    if (recursive == 0 && find->children_num != 0)
        return 0;

    if (current_dir == find)
        current_dir = root;

    int result = delete_child(&find->parent, find->name);
    if (result == 0)
        return 0;

    int del_size = size(path);
    fm_size += del_size != -1 ? del_size : 0;

    return result;
}
int change_dir(char const* path)
{
    if (root == NULL)
        return 0;

    Node* find = find_node(path[0] == '/' ? root : current_dir, path, NULL, 0);
    if (find == NULL || find->size != 0)
        return 0;

    current_dir = find;
    return 1;
}
void get_cur_dir(char* dst)
{
    if (current_dir == NULL || strlen(dst) > 128)
        return;

    full_path(current_dir, &dst);

    if (strlen(dst) != 1)
        dst[strlen(dst) - 1] = '\0';
}
int size(char const* path)
{
    if (root == NULL)
        return -1;

    Node* find = find_node(path[0] == '/' ? root : current_dir, path, NULL, 0);
    if (find == NULL)
        return -1;

    int result = recursive_size(find);
    return result == 0 ? -1 : result;
}

