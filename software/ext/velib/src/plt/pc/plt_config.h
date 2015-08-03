/* Use the task functions */
#define CFG_WITH_TASK					1

/* .. for whatever OS this is build on */
#define CFG_WITH_TASK_NATIVE			1

/* .. without any additional arguments */
#define CFG_WITH_DEFAULT_ARGUMENTS		1

/* .. which should also init the CAN bus during startup */
#define CFG_INIT_CANBUS					1

/* .. whichever is selected by the user / runtime avaialble */
#define CFG_WITH_CANHW_DRIVER			1

/* and provide memory for the loopback in the can driver */
#define CFG_WITH_J1939_SF_MEM			1
