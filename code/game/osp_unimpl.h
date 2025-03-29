
#define OSP2_UNIMPLEMENTED_FUNCTION(NAME)  trap_SendServerCommand(-1, va("print \"Unimplemented function %s called.\n\"", NAME));
#define OSP2_DEPRECATED_FUNCTION(NAME)  trap_SendServerCommand(-1, va("print \"Deprecated function %s called.\n\"", NAME));

extern void    trap_SendServerCommand(int clientNum, const char* text);
