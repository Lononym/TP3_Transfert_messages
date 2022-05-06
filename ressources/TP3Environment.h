#ifndef TP3_Environment
#define TP3_Environment

#ifdef __cplusplus
extern "C" {
#endif
extern void startEnvironment(SEM_ID HPer, MSG_Q_ID HeureReelle);
extern void startEnvironmentPb(SEM_ID HPer, MSG_Q_ID HeureReelle);
extern void stopEnvironment();

#ifdef __cplusplus
}
#endif

typedef struct
	{
		int Heure;
		int Minute;
		int Seconde;
	} type_heure;


#endif
