#include "hw6.h"

int main(int argc, char *argv[]) {

    // Part 1: Validate the Command-line Arguments
    if (argc < 7) {
        return MISSING_ARGUMENT;
    }
    int opt;
    extern char *optarg;
    extern int optind;
    int s_counter = 0;
    int r_counter = 0;
    int w_counter = 0;
    int l_counter = 0;
    char *s_arg = NULL;
    char *r_arg = NULL;

    char *l_arg = NULL;
    int startline = -1;
    int endline = -1;

    while ((opt = getopt(argc, argv, ":s:r:wl:")) != -1) {
        switch(opt) {
            case 's':
                s_counter++;
                if (s_counter > 1) {
                    return DUPLICATE_ARGUMENT;
                }
                s_arg = optarg;
                break;
            
            case 'r':
                r_counter++;
                if (r_counter > 1) {
                    return DUPLICATE_ARGUMENT;
                }
                r_arg = optarg;
                break;
            
            case 'w':
                w_counter++;
                if (w_counter > 1) {
                    return DUPLICATE_ARGUMENT;
                }
                break;
            
            case 'l':
                l_counter++;
                if (l_counter > 1) {
                    return DUPLICATE_ARGUMENT;
                }
                l_arg = optarg;
                break;

            case ':':
                break;

            case '?':
                break;
        }
    }

    if (access(argv[argc - 2], R_OK) != 0) {
        return INPUT_FILE_MISSING;
    }

    FILE *infile; 
    infile = fopen(argv[argc - 2], "r");
    if (infile == NULL) {
        return INPUT_FILE_MISSING;
    }

    FILE *outfile; 
    outfile = fopen(argv[argc - 1], "w");
    if (outfile == NULL) {
        fclose(infile);
        return OUTPUT_FILE_UNWRITABLE;
    }

    if (s_arg == NULL || s_arg[0] == '-') {
        fclose(infile);
        fclose(outfile);
        return S_ARGUMENT_MISSING;
    }
    else {
        if (r_arg == NULL || r_arg[0] == '-') {
            fclose(infile);
            fclose(outfile);
            return R_ARGUMENT_MISSING;
        }
        else {
            if (l_counter == 1 && l_arg != NULL) {
                if (l_arg[0] == '-') {
                    fclose(infile);
                    fclose(outfile);
                    return L_ARGUMENT_INVALID;
                }
            }
        }
    }

    // l checker (L MUST BE VALID)
    if (l_counter == 1) {
        bool comma_flag = false;
        for (int i = 0; i < strlen(l_arg); i++) {
            if (l_arg[i] == ',') {
                comma_flag = true;
                break;
            }
        }

        if (comma_flag == false) {
            fclose(infile);
            fclose(outfile);
            return L_ARGUMENT_INVALID;
        }
        
        char *ptr;
        bool end_flag = false;
        char *token = strtok(l_arg, ",");
        while (token != NULL) {
            if (!isdigit(token[0])) {
                fclose(infile);
                fclose(outfile);
                return L_ARGUMENT_INVALID;
            }
            if (end_flag == false) {
                startline = strtol(token, &ptr, 10);
                end_flag = true;
            }
            else {
                endline = strtol(token, &ptr, 10);
            }
            token = strtok(NULL, ",");
        }
        if (startline > endline || startline < 0 || endline < 0) {
            fclose(infile);
            fclose(outfile);
            return L_ARGUMENT_INVALID;
        }
    }

    if (w_counter == 1) {
        if (strchr(s_arg, '*') == NULL) {
            fclose(infile);
            fclose(outfile);
            return WILDCARD_INVALID;
        }
        else {
            if (s_arg[0] == '*' && s_arg[strlen(s_arg) - 1] == '*') {
                fclose(infile);
                fclose(outfile);
                return WILDCARD_INVALID;
            }
            else {
                if (s_arg[0] != '*' && s_arg[strlen(s_arg) - 1] != '*') {
                    fclose(infile);
                    fclose(outfile);
                    return WILDCARD_INVALID;
                }
            }
        }
    } // Part 1 Complete

    // Part 2: Simple Search and Replacement
    if (w_counter == 0) {
        int line_count = 1;
        while (1) {
            if (feof(infile)) {
                break;
            }
            int check = fgetc(infile);
            if (check == '\n' || check == '\0') {
                line_count++;
            }
        }

        rewind(infile);
        char line[MAX_LINE], *p;

        if (l_counter == 0) {
            for (int i = 0; i < line_count; i++) {
                fgets(line, MAX_LINE, infile);
                char *line_ptr = &line[0];
                while ((p = strstr(line_ptr, s_arg)) != NULL) {
                    int ind = p - line_ptr;
                    fprintf(outfile, "%.*s", ind, line_ptr);
                    fputs(r_arg, outfile);
                    line_ptr = p + strlen(s_arg);
                }
                fputs(line_ptr, outfile);
            }
        }
        else {
            for (int i = 0; i < line_count; i++) {
                fgets(line, MAX_LINE, infile);
                char *line_ptr = &line[0];
                if ((i + 1) >= startline && (i + 1) <= endline) {
                    while ((p = strstr(line_ptr, s_arg)) != NULL) {
                        int ind = p - line_ptr;
                        fprintf(outfile, "%.*s", ind, line_ptr);
                        fputs(r_arg, outfile);
                        line_ptr = p + strlen(s_arg);
                    }
                }
                fputs(line_ptr, outfile);
            }
        } 
    } // Part 2 Complete

    // Part 3: Wildcard Search and Replacement
    if (w_counter == 1) {
        bool front_ast = false;
        if (s_arg[0] == '*') {
            front_ast = true;
        }

        if (front_ast == true) {
            memmove(s_arg, s_arg + 1, strlen(s_arg));
        }
        else {
            s_arg[strlen(s_arg) - 1] = '\0';
        }

        int line_count = 1;
        while (1) {
            if (feof(infile)) {
                break;
            }
            int check = fgetc(infile);
            if (check == '\n' || check == '\0') {
                line_count++;
            }
        }

        rewind(infile);
        char line[MAX_LINE], *p;
        int word_len = 0;
        int temp_ind = 0;

        if (l_counter == 0) {
            for (int i = 0; i < line_count; i++) {
                fgets(line, MAX_LINE, infile);
                char *line_ptr = &line[0];
                while ((p = strstr(line_ptr, s_arg)) != NULL) {
                    word_len = strlen(s_arg); 
                    int ind = p - line_ptr;                   

                    if (front_ast == true) {
                        temp_ind = ind;
                        if (!isalnum(line_ptr[ind + strlen(s_arg)])) {
                            while(ind != 0 && !isspace(line_ptr[ind - 1]) && !ispunct(line_ptr[ind - 1])) {
                                ind--;
                                word_len++;
                                p--;
                            }
                        }
                    }
                    else {
                        word_len = 0;
                        while((line_ptr[ind + word_len] != '\n')  && !isspace(line_ptr[ind + word_len]) && !ispunct(line_ptr[ind + word_len]) && isalnum(line_ptr[ind + word_len])) {
                            word_len++; 
                        }
                    }
                    fprintf(outfile, "%.*s", ind, line_ptr);
                    if ((front_ast == false && ind != 0 && isalnum(line_ptr[ind - 1])) || (front_ast == true && isalnum(line_ptr[temp_ind + strlen(s_arg)]))) {
                        fputs(s_arg, outfile);
                        line_ptr = p + strlen(s_arg);
                    }
                    else {
                        fputs(r_arg, outfile);
                        line_ptr = p + word_len;
                    }
                }
                fputs(line_ptr, outfile);
            }
        }
        else {
            for (int i = 0; i < line_count; i++) {
                fgets(line, MAX_LINE, infile);
                char *line_ptr = &line[0];
                if ((i + 1) >= startline && (i + 1) <= endline) {
                    while ((p = strstr(line_ptr, s_arg)) != NULL) {
                        word_len = strlen(s_arg); 
                        int ind = p - line_ptr;                   
                        if (front_ast == true) {
                            temp_ind = ind;
                            if (!isalnum(line_ptr[ind + strlen(s_arg)])) {
                                while(ind != 0 && !isspace(line_ptr[ind - 1]) && !ispunct(line_ptr[ind - 1])) {
                                    ind--;
                                    word_len++;
                                    p--;
                                }
                            }
                        }
                        else {
                            word_len = 0;
                            while((line_ptr[ind + word_len] != '\n')  && !isspace(line_ptr[ind + word_len]) && !ispunct(line_ptr[ind + word_len]) && isalnum(line_ptr[ind + word_len])) {
                                word_len++; 
                            }
                        }
                        fprintf(outfile, "%.*s", ind, line_ptr);
                        if ((front_ast == false && ind != 0 && isalnum(line_ptr[ind - 1])) || (front_ast == true && isalnum(line_ptr[temp_ind + strlen(s_arg)]))) {
                            fputs(s_arg, outfile);
                            line_ptr = p + strlen(s_arg);
                        }
                        else {
                            fputs(r_arg, outfile);
                            line_ptr = p + word_len;
                        }
                    }
                }
                fputs(line_ptr, outfile);
            }
        }
    } // Part 3 Complete

    fclose(infile);
    fclose(outfile);
    return 0;
}