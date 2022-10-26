#!/bin/sh                                                                       
                                                                                
if [ -z $1 ]; then                                                              
    EXEC_PATH=$PWD                                                              
else                                                                            
    EXEC_PATH=$1                                                                
fi                                                                              
                                                                                
# remove trailing whitespace                                                    
find $EXEC_PATH -type f \( -iname "*.hpp" -or -iname "*.h" -or -iname "*.c" -or -iname "*.cpp" \) -not -path "*/import/*" | xargs sed -e 's/[[:blank:]]\+$//' -i
                                                                                
# convert line endings to unix                                                  
find $EXEC_PATH -type f \( -iname "*.hpp" -or -iname "*.h" -or -iname "*.c" -or -iname "*.cpp" \) -not -path "*/import/*" | xargs dos2unix
                                                                                
# convert tabs to 4 spaces                                                      
find $EXEC_PATH -type f \( -iname "*.hpp" -or -iname "*.h" -or -iname "*.c" -or -iname "*.cpp" \) -not -path "*/import/*" | xargs sed -i 's/\t/    /g'
                                                                                
# format source code                                                            
find $EXEC_PATH -type f \( -iname "*.hpp" -or -iname "*.h" -or -iname "*.c" -or -iname "*.cpp" \) -not -path "*/import/*" | xargs clang-format -i
