/*
small test for swig
*/

/**
 * Create a new context.
 *
 * Every call to this function should be paired with a call to
 * LLVMContextDispose() or the context will leak memory.
 */
extern LLVMContextRef LLVMContextCreate(void);
