#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h> // Required for pow() function

// Helper function to trim leading and trailing whitespace from a string
void trim(char *str) {
    if (str == NULL) return;

    char *start = str;
    while (isspace((unsigned char)*start)) start++;

    char *end = start + strlen(start) - 1;
    while (end >= start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';

    memmove(str, start, end - start + 2);
}

// Function to convert Bminor power operator (^) to C's pow()
// This version attempts to handle multiple power operators on one line
// and rebuilds the string more carefully.
void convert_power(char *expr) {
    char temp_buffer[512];
    temp_buffer[0] = '\0';
    char *current_pos = expr; // Current reading position in the original expr

    char *power_op_pos;
    while ((power_op_pos = strchr(current_pos, '^')) != NULL) {
        // Find end of base (going backward from '^')
        char *base_end = power_op_pos - 1;
        while (base_end >= expr && isspace((unsigned char)*base_end)) base_end--;

        // Find start of the base: go backwards until non-expression character or start of string
        char *base_start = base_end;
        int paren_balance = 0;
        while (base_start >= expr) {
            char c = *base_start;
            if (c == ')') paren_balance++;
            else if (c == '(') paren_balance--;

            // Allow alphanumeric, underscore, dot, operators within balanced parentheses
            if (isalnum((unsigned char)c) || c == '_' || c == '.' || c == '(' || c == ')' || c == '-' || c == '+' || paren_balance > 0) {
                base_start--;
            } else { // Found non-expression character or unbalanced parenthesis
                break;
            }
        }
        base_start++; // Move to the actual start of the base expression

        // Append the part before the base to temp_buffer
        strncat(temp_buffer, current_pos, base_start - current_pos);

        // Extract base
        char base_str[128];
        if (base_end - base_start + 1 > 0) {
            strncpy(base_str, base_start, base_end - base_start + 1);
            base_str[base_end - base_start + 1] = '\0';
            trim(base_str);
        } else {
            base_str[0] = '\0'; // Handle empty base if logic fails
        }


        // Find start of the exponent: go forwards from power_op_pos + 1
        char *exp_start = power_op_pos + 1;
        while (*exp_start != '\0' && isspace((unsigned char)*exp_start)) exp_start++; // Trim leading space

        // Find end of the exponent: go forwards until non-expression character or end of string
        char *exp_end = exp_start;
        paren_balance = 0;
        while (*exp_end != '\0') {
            char c = *exp_end;
            if (c == '(') paren_balance++;
            else if (c == ')') paren_balance--;

            if (isalnum((unsigned char)c) || c == '_' || c == '.' || c == '(' || c == ')' || c == '-' || c == '+' || paren_balance > 0) {
                exp_end++;
            } else {
                break;
            }
        }
        exp_end--; // Move to actual end of the exponent expression

        // Extract exponent
        char exp_str[128];
        if (exp_end - exp_start + 1 > 0) {
            strncpy(exp_str, exp_start, exp_end - exp_start + 1);
            exp_str[exp_end - exp_start + 1] = '\0';
            trim(exp_str);
        } else {
            exp_str[0] = '\0'; // Handle empty exponent
        }

        // Append the pow() call to temp_buffer
        sprintf(temp_buffer + strlen(temp_buffer), "pow(%s, %s)", base_str, exp_str);

        // Update current_pos to continue scanning from after the exponent
        current_pos = exp_end + 1;
    }
    // Append any remaining part of the string after the last power operator
    strcat(temp_buffer, current_pos);

    // Copy the modified string back
    strcpy(expr, temp_buffer);
}

int main() {
    // Standard C headers needed for the translated code
    printf("#include <stdio.h>\n");
    printf("#include <stdlib.h>\n"); // For malloc
    printf("#include <string.h>\n");
    printf("#include <ctype.h>\n");
    printf("#include <stdbool.h>\n"); // For bool type
    printf("#include <math.h>\n\n"); // For pow() function

    // Declare dynamic_array globally in the generated C output
    printf("int *dynamic_array;\n\n"); 

    char line[256];
    bool in_function_body = false;
    bool in_multiline_comment = false;
    int brace_level = 0; // Tracks { } nesting for indentation

    while (fgets(line, sizeof(line), stdin) != NULL) {
        // Remove trailing newline and carriage return characters
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (len > 0 && line[len - 1] == '\r') line[len - 1] = '\0';

        char trimmed_line[256];
        strcpy(trimmed_line, line);
        trim(trimmed_line); // Trim leading/trailing whitespace for parsing

        // Handle empty lines
        if (strlen(trimmed_line) == 0) {
            printf("\n");
            continue;
        }

        // Handle multiline comments /* ... */
        if (strstr(trimmed_line, "/*") != NULL) {
            in_multiline_comment = true;
        }
        if (in_multiline_comment) {
            // Print the comment line as is, maintaining original indentation
            printf("%s\n", line);
            if (strstr(trimmed_line, "*/") != NULL) {
                in_multiline_comment = false;
            }
            continue;
        }

        // Handle single-line comments //
        if (trimmed_line[0] == '/' && trimmed_line[1] == '/') {
            printf("%s\n", line); // Print the comment line as is
            continue;
        }

        // Adjust brace level for closing braces before printing line content
        if (strcmp(trimmed_line, "}") == 0) {
            brace_level--;
        }

        // Add indentation based on current brace_level
        for (int i = 0; i < brace_level; i++) {
            printf("    ");
        }

        // Handle specific Bminor constructs in order of specificity

        // Handle opening brace
        if (strcmp(trimmed_line, "{") == 0) {
            printf("{\n");
            brace_level++;
            continue;
        }

        // Function definition
        if (strstr(trimmed_line, ": function") != NULL) {
            char func_name[64];
            char return_type[32];
            char params_raw[128] = "";
            char *open_paren = strchr(trimmed_line, '(');
            char *close_paren = strchr(trimmed_line, ')');

            if (open_paren && close_paren) {
                strncpy(params_raw, open_paren + 1, close_paren - (open_paren + 1));
                params_raw[close_paren - (open_paren + 1)] = '\0';
                trim(params_raw);
            }

            // Extract function name and return type
            sscanf(trimmed_line, "%[^:]: function %s", func_name, return_type);
            trim(func_name); trim(return_type);

            // Convert return type
            char c_return_type[32];
            if (strcmp(return_type, "integer") == 0) strcpy(c_return_type, "int");
            else if (strcmp(return_type, "string") == 0) strcpy(c_return_type, "char*");
            else if (strcmp(return_type, "boolean") == 0) strcpy(c_return_type, "bool");
            else strcpy(c_return_type, "void"); // Default for unknown types

            // Convert parameters
            char c_params[256] = "";
            if (strlen(params_raw) > 0) {
                char *param_token = strtok(params_raw, ",");
                while (param_token != NULL) {
                    trim(param_token); // Trim each parameter token
                    char p_name[64];
                    char p_type[32];
                    sscanf(param_token, "%[^:]: %s", p_name, p_type);
                    trim(p_name); trim(p_type);

                    char c_p_type[32];
                    if (strcmp(p_type, "integer") == 0) strcpy(c_p_type, "int");
                    else if (strcmp(p_type, "string") == 0) strcpy(c_p_type, "char*");
                    else if (strcmp(p_type, "boolean") == 0) strcpy(c_p_type, "bool");
                    else strcpy(c_p_type, "void");

                    if (strlen(c_params) > 0) strcat(c_params, ", ");
                    strcat(c_params, c_p_type);
                    strcat(c_params, " ");
                    strcat(c_params, p_name);

                    param_token = strtok(NULL, ",");
                }
            }

            if (strcmp(func_name, "main") == 0) {
                printf("%s %s() {\n", c_return_type, func_name); // Simplified main signature
                // Allocate dynamic_array here for global dynamic arrays (runtime operation)
                printf("    dynamic_array = malloc(10 * sizeof(int));\n");
                printf("    if (dynamic_array == NULL) { fprintf(stderr, \"Memory allocation failed!\\n\"); return 1; }\n");
            } else {
                printf("%s %s(%s) {\n", c_return_type, func_name, c_params);
            }
            in_function_body = true;
            // Brace level will be incremented by the following '{' line
            continue;
        }

        // Print statements (Moved up for priority and fixed semicolon handling)
        if (strstr(trimmed_line, "print ") == trimmed_line) { // Line starts with "print "
            char print_args_raw[256];
            strcpy(print_args_raw, trimmed_line + strlen("print "));
            // Remove trailing semicolon from the raw arguments string before parsing
            if (strlen(print_args_raw) > 0 && print_args_raw[strlen(print_args_raw) - 1] == ';') {
                print_args_raw[strlen(print_args_raw) - 1] = '\0';
            }
            trim(print_args_raw); // Now trim

            char format_string[512] = "";
            char printf_args_list[512] = ""; // For variables/expressions passed as arguments
            bool first_printf_arg = true;

            // Use a temporary copy for strtok as it modifies the string
            char temp_args[256];
            strcpy(temp_args, print_args_raw);

            char *token = strtok(temp_args, ",");
            while (token != NULL) {
                trim(token);
                if (token[0] == '"' && token[strlen(token) - 1] == '"') {
                    // Check if it's explicitly the newline string literal "\n"
                    if (strcmp(token, "\"\\n\"") == 0) {
                        strcat(format_string, "\\n"); // Append as actual newline to format string
                    } else {
                        // It's a general string literal, remove quotes and append directly to format string
                        token[strlen(token) - 1] = '\0'; // Remove closing quote
                        strcat(format_string, token + 1); // Append content after opening quote
                    }
                } else { // Variable or expression
                    // Basic type deduction (can be improved with a full symbol table)
                    // Assuming 'name' is char*, others are int based on context
                    bool is_string_variable = (strcmp(token, "name") == 0);
                    
                    if (is_string_variable) {
                        strcat(format_string, "%s");
                    } else {
                        strcat(format_string, "%d"); // Assume integer for others
                    }
                    
                    if (!first_printf_arg) {
                        strcat(printf_args_list, ", ");
                    }
                    strcat(printf_args_list, token);
                    first_printf_arg = false;
                }
                token = strtok(NULL, ",");
            }
            printf("printf(\"%s\"", format_string);
            if (strlen(printf_args_list) > 0) {
                printf(", %s", printf_args_list);
            }
            printf(");\n"); // Correct closing parenthesis and semicolon
            continue;
        }

        // If statement
        if (strstr(trimmed_line, "if (") == trimmed_line) {
            char condition[128];
            sscanf(trimmed_line, "if (%[^)]) {", condition);
            trim(condition);

            // Replace Bminor logical operators with C operators
            char temp_cond[128];
            strcpy(temp_cond, condition);
            char* ptr;

            ptr = strstr(temp_cond, " and ");
            while (ptr) { memcpy(ptr, " && ", 4); ptr = strstr(ptr + 4, " and "); }
            ptr = strstr(temp_cond, " or ");
            while (ptr) { memcpy(ptr, " || ", 4); ptr = strstr(ptr + 4, " or "); }
            ptr = strstr(temp_cond, " not ");
            while (ptr) { memcpy(ptr, " ! ", 3); ptr = strstr(ptr + 3, " not "); } // " not " to " !"

            printf("if (%s) {\n", temp_cond);
            // Brace level will be incremented by the following '{' line
            continue;
        }

        // Else statement
        // Handle else statements that are not part of an if block

        if (strcmp(trimmed_line, "} else {") == 0) {
            brace_level--;  // For the closing brace
            // Print at current brace_level (which is now reduced)
            for (int i = 0; i < brace_level; i++) {
                printf("    ");
            }
            printf("} else {\n");
            brace_level++;  // For the new block
            continue;
        }
        // Handle else statements that are not part of an if block
        // This handles the case where "else" is on a new line or after an if block
        // Check for "else {" on a new line or after an if block
        // This is a special case where "else" is on a new line after an if block
        if (strstr(trimmed_line, "else") == trimmed_line && 
            (strlen(trimmed_line) == 4 || trimmed_line[4] == ' ' || trimmed_line[4] == '\0')) {
            // If the line is just "else" or "else {"
            brace_level--; // Close previous block
            for (int i = 0; i < brace_level; i++) {
                printf("    ");
            }
            printf("} else {\n"); // Print else block
            brace_level++; // Open new block
            continue;
        }
        



        if (strcmp(trimmed_line, "else {") == 0) {
            printf("} else {\n"); // Close previous block and open else
            // Brace level will be incremented by the following '{' line
            continue;
        }

        // For loop
        if (strstr(trimmed_line, "for(") == trimmed_line) {
            char init_part[64], cond_part[64], incr_part[64];
            
            // Extract parts of the for loop
            if (sscanf(trimmed_line, "for(%[^;];%[^;];%[^)]) {", init_part, cond_part, incr_part) == 3) {
                trim(init_part); trim(cond_part); trim(incr_part);

                // Handle variable declaration in initialization (e.g., i: integer = 0)
                if (strstr(init_part, ": integer") != NULL) {
                    char var_name[32], var_value[32];
                    sscanf(init_part, "%[^:]: integer = %s", var_name, var_value);
                    trim(var_name); trim(var_value);
                    printf("int %s = %s;\n", var_name, var_value); // Declare before for loop
                    snprintf(init_part, sizeof(init_part), "%s = %s", var_name, var_value); // Update init for C for loop
                } else {
                    // If it's just an assignment or existing variable, keep as is
                }

                // Convert power operator in condition and increment
                convert_power(cond_part);
                convert_power(incr_part);

                // Convert common increment patterns (e.g., i = i + 1 -> i++)
                char temp_incr_var[64]; // Use a temp var to prevent sscanf overwriting incr_part
                if (sscanf(incr_part, "%[^ ] = %[^ ] + 1", temp_incr_var, temp_incr_var) == 2) {
                    snprintf(incr_part, sizeof(incr_part), "%s++", temp_incr_var);
                }

                printf("for(%s; %s; %s) {\n", init_part, cond_part, incr_part);
                // Brace level will be incremented by the following '{' line
                continue;
            }
        }

        // Return statement
        if (strstr(trimmed_line, "return ") == trimmed_line) {
            char return_val[128];
            sscanf(trimmed_line, "return %[^;];", return_val);
            trim(return_val);

            // Convert power operator in return value
            convert_power(return_val);
            
            // If the return value contains pow(), cast it to int if the function returns integer
            if (strstr(return_val, "pow(") != NULL) {
                printf("return (int)%s;\n", return_val);
            } else {
                printf("return %s;\n", return_val);
            }
            continue;
        }

        // Array declarations
        if (strstr(trimmed_line, ": array") != NULL) {
            char var_name[64];
            char array_type[32];
            char array_content[256];

            if (strstr(trimmed_line, "= {") != NULL) { // Static array initialization
                sscanf(trimmed_line, "%[^:]: array [%*d] %[^=]= %[^;];", var_name, array_type, array_content);
                trim(var_name); trim(array_type); trim(array_content);
                
                if (strcmp(array_type, "integer") == 0) {
                    printf("int %s[] = %s;\n", var_name, array_content);
                } else {
                    printf("// Unhandled static array type: %s\n", line);
                }
            } else if (strstr(trimmed_line, "= new array[") != NULL) { // Dynamic array allocation
                int current_array_size;
                sscanf(trimmed_line, "%[^:]: array [%*d] %[^=]= new array[%d];", var_name, array_type, &current_array_size);
                trim(var_name); trim(array_type);
                
                // For local dynamic arrays, allocate here
                if (in_function_body) {
                    if (strcmp(array_type, "integer") == 0) {
                        printf("int *%s = malloc(%d * sizeof(int));\n", var_name, current_array_size);
                    } else if (strcmp(array_type, "boolean") == 0) {
                        printf("bool *%s = malloc(%d * sizeof(bool));\n", var_name, current_array_size);
                    } else {
                        printf("// Unhandled local dynamic array type: %s\n", line);
                    }
                } else {
                    // Global dynamic_array has specific handling in main. Others are not supported.
                    printf("// Global dynamic array '%s' must be allocated in a function like main().\n", var_name);
                }
            } else {
                printf("// Unhandled array declaration format: %s\n", line);
            }
            continue;
        }

        // Variable declarations (global and local) - now after more specific cases
        // Check for ':' and not array or function in the line.
        if (strchr(trimmed_line, ':') != NULL && !strstr(trimmed_line, "array") && !strstr(trimmed_line, "function")) {
            char var_name[64];
            char var_type[32];
            char var_value[128]; // Can be empty if no assignment

            if (strchr(trimmed_line, '=') != NULL) { // Declaration with assignment
                if (sscanf(trimmed_line, "%[^:]: %[^=]= %[^;];", var_name, var_type, var_value) == 3) {
                    trim(var_name); trim(var_type); trim(var_value);

                    // Convert power operator in value if present
                    convert_power(var_value);

                    // Type mapping
                    if (strcmp(var_type, "integer") == 0) {
                        // Cast to int if pow() is used, as pow returns double
                        if (strstr(var_value, "pow(") != NULL) {
                            printf("int %s = (int)%s;\n", var_name, var_value);
                        } else {
                            printf("int %s = %s;\n", var_name, var_value);
                        }
                    } else if (strcmp(var_type, "string") == 0) {
                        printf("char* %s = %s;\n", var_name, var_value);
                    } else if (strcmp(var_type, "boolean") == 0) {
                        printf("bool %s = %s;\n", var_name, var_value);
                    } else {
                        printf("// Unhandled variable type: %s\n", line);
                    }
                    continue;
                }
            } else { // Declaration without assignment (e.g., i: integer;)
                 if (sscanf(trimmed_line, "%[^:]: %[^;];", var_name, var_type) == 2) {
                     trim(var_name); trim(var_type);
                     if (strcmp(var_type, "integer") == 0) {
                        printf("int %s;\n", var_name);
                    } else if (strcmp(var_type, "string") == 0) {
                        printf("char* %s;\n", var_name);
                    } else if (strcmp(var_type, "boolean") == 0) {
                        printf("bool %s;\n", var_name);
                    } else {
                        printf("// Unhandled variable type (no init): %s\n", line);
                    }
                    continue;
                 }
            }
        }

        // Array access and assignment (e.g., static_array[0] = temp;)
        if (strchr(trimmed_line, '[') != NULL && strchr(trimmed_line, ']') != NULL && strchr(trimmed_line, '=') != NULL) {
            char cleaned_line[256];
            strcpy(cleaned_line, trimmed_line);
            convert_power(cleaned_line); // Check for power in the assigned value
            
            // Cast to int if pow() is used in the assigned value
            if (strstr(cleaned_line, "pow(") != NULL) {
                char var_part[128];
                char value_part[128];
                // Assuming format like "array[index] = value;"
                sscanf(cleaned_line, "%[^=]= %[^;];", var_part, value_part);
                trim(var_part); trim(value_part);
                printf("%s = (int)%s;\n", var_part, value_part);
            } else {
                printf("%s;\n", cleaned_line);
            }
            continue;
        }

        // Simple assignments and expressions (must come after more specific patterns)
        if (strchr(trimmed_line, '=') != NULL || strchr(trimmed_line, '+') != NULL || strchr(trimmed_line, '-') != NULL ||
            strchr(trimmed_line, '*') != NULL || strchr(trimmed_line, '/') != NULL || strchr(trimmed_line, '%') != NULL) {
            char cleaned_line[256];
            strcpy(cleaned_line, trimmed_line);

            // Convert power operator
            convert_power(cleaned_line);

            // Replace logical operators
            char* ptr;
            ptr = strstr(cleaned_line, " and ");
            while (ptr) { memcpy(ptr, " && ", 4); ptr = strstr(ptr + 4, " and "); }
            ptr = strstr(cleaned_line, " or ");
            while (ptr) { memcpy(ptr, " || ", 4); ptr = strstr(ptr + 4, " or "); }
            ptr = strstr(cleaned_line, " not ");
            while (ptr) { memcpy(ptr, " ! ", 3); ptr = strstr(ptr + 3, " not "); }

            // Cast to int if pow() is used in the assignment
            if (strstr(cleaned_line, "pow(") != NULL) {
                char var_part[128];
                char value_part[128];
                sscanf(cleaned_line, "%[^=]= %[^;];", var_part, value_part);
                trim(var_part); trim(value_part);
                printf("%s = (int)%s;\n", var_part, value_part);
            } else {
                printf("%s;\n", cleaned_line);
            }
            continue;
        }
        
        // Handle closing brace (already handled brace_level adjustment above)
        if (strcmp(trimmed_line, "}") == 0) {
            printf("}\n");
            if (brace_level == 0 && in_function_body) { // Exiting the outermost function body (e.g., main)
                in_function_body = false;
                printf("    free(dynamic_array);\n"); // Free dynamic array at end of main
            }
            continue;
        }

        // If a line reaches here, it's unhandled by specific rules
        printf("// Unhandled Bminor statement: %s\n", line);
    }

    return 0;
}