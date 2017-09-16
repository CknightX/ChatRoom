#ifndef DBG_H
#define DBG_H


#ifdef DBGON
#define debug(msg,...) fprintf(stderr, "<<DEBUG>>(%s:%d)" msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(msg,...)
#endif
//err
#define log_err(msg,...) fprintf(stderr, "[ERROR](%s:%d)" msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
//info
#define log_info(msg,...) fprintf(stderr, "[INFO](%s:%d)" msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)

//check 
#define check_exit(exp,msg,...) do {if (!(exp)) {log_err(msg,##__VA_ARGS__); exit(0);}} while(0)

#endif