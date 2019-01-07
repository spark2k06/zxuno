#define	FAM_DAT_P1	(1<<5)
#define	FAM_LAT_P1	(1<<2)
#define	FAM_CLK_P1	(1<<3)

#define	FAM_DAT_P2	(1<<3)
#define	FAM_LAT_P2	(1<<7)
#define	FAM_CLK_P2	(1<<2)

uchar CheckFamicomP1();
uchar CheckFamicomP2();

void ReadFamicomP1(report_t *p1);
uchar FamicomReadByteP1(void);

void ReadFamicomP2(report_t *p2);
uchar FamicomReadByteP2(void);
