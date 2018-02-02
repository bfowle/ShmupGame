alias bit bool;
extern (C) {
alias int BulletMLParserTinyXML;
int* BulletMLParserTinyXML_new(char*);
void BulletMLParserTinyXML_parse(int* );
void BulletMLParserTinyXML_delete(int*);
alias int BulletMLParser;
alias int BulletMLState;
alias int BulletMLRunner;
alias int BulletMLRunnerD;
int* BulletMLRunner_new_parser(BulletMLParser*);
int* BulletMLRunner_new_state(BulletMLState*);
void BulletMLRunner_delete(int*);
void BulletMLRunner_run(int* );
bool BulletMLRunner_isEnd(int* );
void BulletMLRunner_set_getBulletDirection(int*, double (*fp) (int* )); 
void BulletMLRunner_set_getAimDirection(int*, double (*fp) (int* )); 
void BulletMLRunner_set_getBulletSpeed(int*, double (*fp) (int* )); 
void BulletMLRunner_set_getDefaultSpeed(int*, double (*fp) (int* )); 
void BulletMLRunner_set_getRank(int*, double (*fp) (int* )); 
void BulletMLRunner_set_createSimpleBullet(int*, void (*fp) (int* , double, double)); 
void BulletMLRunner_set_createBullet(int*, void (*fp) (int* , BulletMLState*, double, double)); 
void BulletMLRunner_set_getTurn(int*, int (*fp) (int* )); 
void BulletMLRunner_set_doVanish(int*, void (*fp) (int* )); 
void BulletMLRunner_set_doChangeDirection(int*, void (*fp) (int* , double)); 
void BulletMLRunner_set_doChangeSpeed(int*, void (*fp) (int* , double)); 
void BulletMLRunner_set_doAccelX(int*, void (*fp) (int* , double)); 
void BulletMLRunner_set_doAccelY(int*, void (*fp) (int* , double)); 
void BulletMLRunner_set_getBulletSpeedX(int*, double (*fp) (int* )); 
void BulletMLRunner_set_getBulletSpeedY(int*, double (*fp) (int* )); 
void BulletMLRunner_set_getRand(int*, double (*fp) (int* )); 
}
