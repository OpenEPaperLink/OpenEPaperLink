/*
 * pic16fam.h - PIC14 families
 *
 * This file is has been generated using ./pic16fam-h-gen.pl .
 */
#ifndef __SDCC_PIC16FAM_H__
#define __SDCC_PIC16FAM_H__ 1

/*
 * Define device class.
 */
#undef  __SDCC_PIC14_ENHANCED

#if   defined(__SDCC_PIC12F1501) || \
      defined(__SDCC_PIC12F1571) || \
      defined(__SDCC_PIC12F1572) || \
      defined(__SDCC_PIC12F1612) || \
      defined(__SDCC_PIC12F1822) || \
      defined(__SDCC_PIC12F1840) || \
      defined(__SDCC_PIC12LF1552) || \
      defined(__SDCC_PIC16F1454) || \
      defined(__SDCC_PIC16F1455) || \
      defined(__SDCC_PIC16F1458) || \
      defined(__SDCC_PIC16F1459) || \
      defined(__SDCC_PIC16F1503) || \
      defined(__SDCC_PIC16F1507) || \
      defined(__SDCC_PIC16F1508) || \
      defined(__SDCC_PIC16F1509) || \
      defined(__SDCC_PIC16F1512) || \
      defined(__SDCC_PIC16F1513) || \
      defined(__SDCC_PIC16F1516) || \
      defined(__SDCC_PIC16F1517) || \
      defined(__SDCC_PIC16F1518) || \
      defined(__SDCC_PIC16F1519) || \
      defined(__SDCC_PIC16F1526) || \
      defined(__SDCC_PIC16F1527) || \
      defined(__SDCC_PIC16F1613) || \
      defined(__SDCC_PIC16F1703) || \
      defined(__SDCC_PIC16F1704) || \
      defined(__SDCC_PIC16F1705) || \
      defined(__SDCC_PIC16F1707) || \
      defined(__SDCC_PIC16F1708) || \
      defined(__SDCC_PIC16F1709) || \
      defined(__SDCC_PIC16F1713) || \
      defined(__SDCC_PIC16F1716) || \
      defined(__SDCC_PIC16F1717) || \
      defined(__SDCC_PIC16F1718) || \
      defined(__SDCC_PIC16F1719) || \
      defined(__SDCC_PIC16F1782) || \
      defined(__SDCC_PIC16F1783) || \
      defined(__SDCC_PIC16F1784) || \
      defined(__SDCC_PIC16F1786) || \
      defined(__SDCC_PIC16F1787) || \
      defined(__SDCC_PIC16F1788) || \
      defined(__SDCC_PIC16F1789) || \
      defined(__SDCC_PIC16F1823) || \
      defined(__SDCC_PIC16F1824) || \
      defined(__SDCC_PIC16F1825) || \
      defined(__SDCC_PIC16F1826) || \
      defined(__SDCC_PIC16F1827) || \
      defined(__SDCC_PIC16F1828) || \
      defined(__SDCC_PIC16F1829) || \
      defined(__SDCC_PIC16F1847) || \
      defined(__SDCC_PIC16F1933) || \
      defined(__SDCC_PIC16F1934) || \
      defined(__SDCC_PIC16F1936) || \
      defined(__SDCC_PIC16F1937) || \
      defined(__SDCC_PIC16F1938) || \
      defined(__SDCC_PIC16F1939) || \
      defined(__SDCC_PIC16F1946) || \
      defined(__SDCC_PIC16F1947) || \
      defined(__SDCC_PIC16LF1902) || \
      defined(__SDCC_PIC16LF1903) || \
      defined(__SDCC_PIC16LF1904) || \
      defined(__SDCC_PIC16LF1906) || \
      defined(__SDCC_PIC16LF1907)
#define __SDCC_PIC14_ENHANCED 1

#endif

/*
 * Define ADC style per device family.
 */
#undef  __SDCC_ADC_STYLE

#if   defined(__SDCC_PIC12F609) || \
      defined(__SDCC_PIC12F629) || \
      defined(__SDCC_PIC12F635) || \
      defined(__SDCC_PIC12LF1552) || \
      defined(__SDCC_PIC16C62) || \
      defined(__SDCC_PIC16C63A) || \
      defined(__SDCC_PIC16C65B) || \
      defined(__SDCC_PIC16C432) || \
      defined(__SDCC_PIC16C554) || \
      defined(__SDCC_PIC16C557) || \
      defined(__SDCC_PIC16C558) || \
      defined(__SDCC_PIC16C620) || \
      defined(__SDCC_PIC16C620A) || \
      defined(__SDCC_PIC16C621) || \
      defined(__SDCC_PIC16C621A) || \
      defined(__SDCC_PIC16C622) || \
      defined(__SDCC_PIC16C622A) || \
      defined(__SDCC_PIC16CR73) || \
      defined(__SDCC_PIC16CR74) || \
      defined(__SDCC_PIC16CR76) || \
      defined(__SDCC_PIC16CR77) || \
      defined(__SDCC_PIC16CR620A) || \
      defined(__SDCC_PIC16F84) || \
      defined(__SDCC_PIC16F84A) || \
      defined(__SDCC_PIC16F87) || \
      defined(__SDCC_PIC16F610) || \
      defined(__SDCC_PIC16F627) || \
      defined(__SDCC_PIC16F627A) || \
      defined(__SDCC_PIC16F628) || \
      defined(__SDCC_PIC16F628A) || \
      defined(__SDCC_PIC16F630) || \
      defined(__SDCC_PIC16F631) || \
      defined(__SDCC_PIC16F636) || \
      defined(__SDCC_PIC16F639) || \
      defined(__SDCC_PIC16F648) || \
      defined(__SDCC_PIC16F648A) || \
      defined(__SDCC_PIC16F1454) || \
      defined(__SDCC_PIC16F1458) || \
      defined(__SDCC_PIC16HV610)
#define __SDCC_ADC_STYLE      0

#elif defined(__SDCC_PIC10F320) || \
      defined(__SDCC_PIC10F322)
#define __SDCC_ADC_STYLE      1003201

#elif defined(__SDCC_PIC12F615) || \
      defined(__SDCC_PIC12F617)
#define __SDCC_ADC_STYLE      1206151

#elif defined(__SDCC_PIC12F675) || \
      defined(__SDCC_PIC12F683)
#define __SDCC_ADC_STYLE      1206751

#elif defined(__SDCC_PIC12F752)
#define __SDCC_ADC_STYLE      1207521

#elif defined(__SDCC_PIC12F1501)
#define __SDCC_ADC_STYLE      1215011

#elif defined(__SDCC_PIC12F1571) || \
      defined(__SDCC_PIC12F1572)
#define __SDCC_ADC_STYLE      1215711

#elif defined(__SDCC_PIC12F1612)
#define __SDCC_ADC_STYLE      1216121

#elif defined(__SDCC_PIC12F1822) || \
      defined(__SDCC_PIC12F1840)
#define __SDCC_ADC_STYLE      1218221

#elif defined(__SDCC_PIC16C71) || \
      defined(__SDCC_PIC16C710) || \
      defined(__SDCC_PIC16C711)
#define __SDCC_ADC_STYLE      1600711

#elif defined(__SDCC_PIC16C72) || \
      defined(__SDCC_PIC16C73B) || \
      defined(__SDCC_PIC16F72) || \
      defined(__SDCC_PIC16F73) || \
      defined(__SDCC_PIC16F76)
#define __SDCC_ADC_STYLE      1600721

#elif defined(__SDCC_PIC16C74B) || \
      defined(__SDCC_PIC16F74) || \
      defined(__SDCC_PIC16F77)
#define __SDCC_ADC_STYLE      1600741

#elif defined(__SDCC_PIC16F88)
#define __SDCC_ADC_STYLE      1600881

#elif defined(__SDCC_PIC16C433)
#define __SDCC_ADC_STYLE      1604331

#elif defined(__SDCC_PIC16F616) || \
      defined(__SDCC_PIC16HV616)
#define __SDCC_ADC_STYLE      1606161

#elif defined(__SDCC_PIC16F676) || \
      defined(__SDCC_PIC16F684) || \
      defined(__SDCC_PIC16F688)
#define __SDCC_ADC_STYLE      1606761

#elif defined(__SDCC_PIC16F677) || \
      defined(__SDCC_PIC16F685) || \
      defined(__SDCC_PIC16F687) || \
      defined(__SDCC_PIC16F689) || \
      defined(__SDCC_PIC16F690)
#define __SDCC_ADC_STYLE      1606771

#elif defined(__SDCC_PIC16F707)
#define __SDCC_ADC_STYLE      1607071

#elif defined(__SDCC_PIC16C715)
#define __SDCC_ADC_STYLE      1607151

#elif defined(__SDCC_PIC16F716)
#define __SDCC_ADC_STYLE      1607161

#elif defined(__SDCC_PIC16C717) || \
      defined(__SDCC_PIC16C770) || \
      defined(__SDCC_PIC16C771)
#define __SDCC_ADC_STYLE      1607171

#elif defined(__SDCC_PIC16F720) || \
      defined(__SDCC_PIC16F721)
#define __SDCC_ADC_STYLE      1607201

#elif defined(__SDCC_PIC16F722) || \
      defined(__SDCC_PIC16F722A) || \
      defined(__SDCC_PIC16F723) || \
      defined(__SDCC_PIC16F723A) || \
      defined(__SDCC_PIC16F726)
#define __SDCC_ADC_STYLE      1607221

#elif defined(__SDCC_PIC16F724) || \
      defined(__SDCC_PIC16F727)
#define __SDCC_ADC_STYLE      1607241

#elif defined(__SDCC_PIC16F737) || \
      defined(__SDCC_PIC16F767)
#define __SDCC_ADC_STYLE      1607371

#elif defined(__SDCC_PIC16C745)
#define __SDCC_ADC_STYLE      1607451

#elif defined(__SDCC_PIC16F747) || \
      defined(__SDCC_PIC16F777)
#define __SDCC_ADC_STYLE      1607471

#elif defined(__SDCC_PIC16F753) || \
      defined(__SDCC_PIC16HV753)
#define __SDCC_ADC_STYLE      1607531

#elif defined(__SDCC_PIC16C765)
#define __SDCC_ADC_STYLE      1607651

#elif defined(__SDCC_PIC16C773)
#define __SDCC_ADC_STYLE      1607731

#elif defined(__SDCC_PIC16C774)
#define __SDCC_ADC_STYLE      1607741

#elif defined(__SDCC_PIC16C781) || \
      defined(__SDCC_PIC16C782)
#define __SDCC_ADC_STYLE      1607811

#elif defined(__SDCC_PIC16F785) || \
      defined(__SDCC_PIC16HV785)
#define __SDCC_ADC_STYLE      1607851

#elif defined(__SDCC_PIC16F818) || \
      defined(__SDCC_PIC16F819)
#define __SDCC_ADC_STYLE      1608181

#elif defined(__SDCC_PIC16F871) || \
      defined(__SDCC_PIC16F874) || \
      defined(__SDCC_PIC16F874A) || \
      defined(__SDCC_PIC16F877) || \
      defined(__SDCC_PIC16F877A)
#define __SDCC_ADC_STYLE      1608711

#elif defined(__SDCC_PIC16F882) || \
      defined(__SDCC_PIC16F883) || \
      defined(__SDCC_PIC16F886)
#define __SDCC_ADC_STYLE      1608821

#elif defined(__SDCC_PIC16F884) || \
      defined(__SDCC_PIC16F887)
#define __SDCC_ADC_STYLE      1608841

#elif defined(__SDCC_PIC16F913) || \
      defined(__SDCC_PIC16F916)
#define __SDCC_ADC_STYLE      1609131

#elif defined(__SDCC_PIC16F914) || \
      defined(__SDCC_PIC16F917) || \
      defined(__SDCC_PIC16F946)
#define __SDCC_ADC_STYLE      1609141

#elif defined(__SDCC_PIC16C925) || \
      defined(__SDCC_PIC16C926) || \
      defined(__SDCC_PIC16F870) || \
      defined(__SDCC_PIC16F872) || \
      defined(__SDCC_PIC16F873) || \
      defined(__SDCC_PIC16F873A) || \
      defined(__SDCC_PIC16F876) || \
      defined(__SDCC_PIC16F876A)
#define __SDCC_ADC_STYLE      1609251

#elif defined(__SDCC_PIC16F1455)
#define __SDCC_ADC_STYLE      1614551

#elif defined(__SDCC_PIC16F1459)
#define __SDCC_ADC_STYLE      1614591

#elif defined(__SDCC_PIC16F1503)
#define __SDCC_ADC_STYLE      1615031

#elif defined(__SDCC_PIC16F1507) || \
      defined(__SDCC_PIC16F1508) || \
      defined(__SDCC_PIC16F1509)
#define __SDCC_ADC_STYLE      1615071

#elif defined(__SDCC_PIC16F1512) || \
      defined(__SDCC_PIC16F1513) || \
      defined(__SDCC_PIC16F1516) || \
      defined(__SDCC_PIC16F1518)
#define __SDCC_ADC_STYLE      1615121

#elif defined(__SDCC_PIC16F1517) || \
      defined(__SDCC_PIC16F1519)
#define __SDCC_ADC_STYLE      1615171

#elif defined(__SDCC_PIC16F1526) || \
      defined(__SDCC_PIC16F1527)
#define __SDCC_ADC_STYLE      1615261

#elif defined(__SDCC_PIC16F1613)
#define __SDCC_ADC_STYLE      1616131

#elif defined(__SDCC_PIC16F1703)
#define __SDCC_ADC_STYLE      1617031

#elif defined(__SDCC_PIC16F1704) || \
      defined(__SDCC_PIC16F1705)
#define __SDCC_ADC_STYLE      1617041

#elif defined(__SDCC_PIC16F1707)
#define __SDCC_ADC_STYLE      1617071

#elif defined(__SDCC_PIC16F1708)
#define __SDCC_ADC_STYLE      1617081

#elif defined(__SDCC_PIC16F1709)
#define __SDCC_ADC_STYLE      1617091

#elif defined(__SDCC_PIC16F1713) || \
      defined(__SDCC_PIC16F1716) || \
      defined(__SDCC_PIC16F1718)
#define __SDCC_ADC_STYLE      1617131

#elif defined(__SDCC_PIC16F1717) || \
      defined(__SDCC_PIC16F1719)
#define __SDCC_ADC_STYLE      1617171

#elif defined(__SDCC_PIC16F1782) || \
      defined(__SDCC_PIC16F1783) || \
      defined(__SDCC_PIC16F1786)
#define __SDCC_ADC_STYLE      1617821

#elif defined(__SDCC_PIC16F1784) || \
      defined(__SDCC_PIC16F1787)
#define __SDCC_ADC_STYLE      1617841

#elif defined(__SDCC_PIC16F1788)
#define __SDCC_ADC_STYLE      1617881

#elif defined(__SDCC_PIC16F1789)
#define __SDCC_ADC_STYLE      1617891

#elif defined(__SDCC_PIC16F1823)
#define __SDCC_ADC_STYLE      1618231

#elif defined(__SDCC_PIC16F1824) || \
      defined(__SDCC_PIC16F1825)
#define __SDCC_ADC_STYLE      1618241

#elif defined(__SDCC_PIC16F1826) || \
      defined(__SDCC_PIC16F1827) || \
      defined(__SDCC_PIC16F1847)
#define __SDCC_ADC_STYLE      1618261

#elif defined(__SDCC_PIC16F1828) || \
      defined(__SDCC_PIC16F1829)
#define __SDCC_ADC_STYLE      1618281

#elif defined(__SDCC_PIC16LF1902) || \
      defined(__SDCC_PIC16LF1903) || \
      defined(__SDCC_PIC16LF1906)
#define __SDCC_ADC_STYLE      1619021

#elif defined(__SDCC_PIC16LF1904) || \
      defined(__SDCC_PIC16LF1907)
#define __SDCC_ADC_STYLE      1619041

#elif defined(__SDCC_PIC16F1933) || \
      defined(__SDCC_PIC16F1936) || \
      defined(__SDCC_PIC16F1938)
#define __SDCC_ADC_STYLE      1619331

#elif defined(__SDCC_PIC16F1934) || \
      defined(__SDCC_PIC16F1937) || \
      defined(__SDCC_PIC16F1939)
#define __SDCC_ADC_STYLE      1619341

#elif defined(__SDCC_PIC16F1946) || \
      defined(__SDCC_PIC16F1947)
#define __SDCC_ADC_STYLE      1619461

#else
#warning No ADC style associated with the target device.
#warning Please update your pic14/pic16fam.h manually and/or inform the maintainer.
#endif

/*
 * Define PWM style per device family.
 */
#undef  __SDCC_PWM_STYLE

#if   defined(__SDCC_PIC12F609) || \
      defined(__SDCC_PIC12F629) || \
      defined(__SDCC_PIC12F635) || \
      defined(__SDCC_PIC12F675) || \
      defined(__SDCC_PIC12LF1552) || \
      defined(__SDCC_PIC16C71) || \
      defined(__SDCC_PIC16C432) || \
      defined(__SDCC_PIC16C433) || \
      defined(__SDCC_PIC16C554) || \
      defined(__SDCC_PIC16C557) || \
      defined(__SDCC_PIC16C558) || \
      defined(__SDCC_PIC16C620) || \
      defined(__SDCC_PIC16C620A) || \
      defined(__SDCC_PIC16C621) || \
      defined(__SDCC_PIC16C621A) || \
      defined(__SDCC_PIC16C622) || \
      defined(__SDCC_PIC16C622A) || \
      defined(__SDCC_PIC16C710) || \
      defined(__SDCC_PIC16C711) || \
      defined(__SDCC_PIC16C715) || \
      defined(__SDCC_PIC16C781) || \
      defined(__SDCC_PIC16C782) || \
      defined(__SDCC_PIC16CR73) || \
      defined(__SDCC_PIC16CR74) || \
      defined(__SDCC_PIC16CR76) || \
      defined(__SDCC_PIC16CR77) || \
      defined(__SDCC_PIC16CR620A) || \
      defined(__SDCC_PIC16F84) || \
      defined(__SDCC_PIC16F84A) || \
      defined(__SDCC_PIC16F610) || \
      defined(__SDCC_PIC16F630) || \
      defined(__SDCC_PIC16F631) || \
      defined(__SDCC_PIC16F636) || \
      defined(__SDCC_PIC16F639) || \
      defined(__SDCC_PIC16F648) || \
      defined(__SDCC_PIC16F676) || \
      defined(__SDCC_PIC16F677) || \
      defined(__SDCC_PIC16F687) || \
      defined(__SDCC_PIC16F688) || \
      defined(__SDCC_PIC16F689) || \
      defined(__SDCC_PIC16F1458) || \
      defined(__SDCC_PIC16HV610) || \
      defined(__SDCC_PIC16LF1902) || \
      defined(__SDCC_PIC16LF1903) || \
      defined(__SDCC_PIC16LF1904) || \
      defined(__SDCC_PIC16LF1906) || \
      defined(__SDCC_PIC16LF1907)
#define __SDCC_PWM_STYLE      0

#elif defined(__SDCC_PIC10F320) || \
      defined(__SDCC_PIC10F322)
#define __SDCC_PWM_STYLE      1003202

#elif defined(__SDCC_PIC12F615) || \
      defined(__SDCC_PIC12F617)
#define __SDCC_PWM_STYLE      1206152

#elif defined(__SDCC_PIC12F683)
#define __SDCC_PWM_STYLE      1206832

#elif defined(__SDCC_PIC12F752)
#define __SDCC_PWM_STYLE      1207522

#elif defined(__SDCC_PIC12F1501)
#define __SDCC_PWM_STYLE      1215012

#elif defined(__SDCC_PIC12F1571) || \
      defined(__SDCC_PIC12F1572)
#define __SDCC_PWM_STYLE      1215712

#elif defined(__SDCC_PIC12F1612)
#define __SDCC_PWM_STYLE      1216122

#elif defined(__SDCC_PIC12F1822) || \
      defined(__SDCC_PIC12F1840)
#define __SDCC_PWM_STYLE      1218222

#elif defined(__SDCC_PIC16C62) || \
      defined(__SDCC_PIC16C72) || \
      defined(__SDCC_PIC16C925) || \
      defined(__SDCC_PIC16C926) || \
      defined(__SDCC_PIC16F72) || \
      defined(__SDCC_PIC16F870) || \
      defined(__SDCC_PIC16F871) || \
      defined(__SDCC_PIC16F872)
#define __SDCC_PWM_STYLE      1600622

#elif defined(__SDCC_PIC16C63A) || \
      defined(__SDCC_PIC16C65B) || \
      defined(__SDCC_PIC16C73B) || \
      defined(__SDCC_PIC16C74B) || \
      defined(__SDCC_PIC16C745) || \
      defined(__SDCC_PIC16C765) || \
      defined(__SDCC_PIC16C773) || \
      defined(__SDCC_PIC16C774) || \
      defined(__SDCC_PIC16F73) || \
      defined(__SDCC_PIC16F74) || \
      defined(__SDCC_PIC16F76) || \
      defined(__SDCC_PIC16F77) || \
      defined(__SDCC_PIC16F873) || \
      defined(__SDCC_PIC16F873A) || \
      defined(__SDCC_PIC16F874) || \
      defined(__SDCC_PIC16F874A) || \
      defined(__SDCC_PIC16F876) || \
      defined(__SDCC_PIC16F876A) || \
      defined(__SDCC_PIC16F877) || \
      defined(__SDCC_PIC16F877A)
#define __SDCC_PWM_STYLE      1600632

#elif defined(__SDCC_PIC16F87) || \
      defined(__SDCC_PIC16F88)
#define __SDCC_PWM_STYLE      1600872

#elif defined(__SDCC_PIC16F616) || \
      defined(__SDCC_PIC16F684) || \
      defined(__SDCC_PIC16HV616)
#define __SDCC_PWM_STYLE      1606162

#elif defined(__SDCC_PIC16F627) || \
      defined(__SDCC_PIC16F627A) || \
      defined(__SDCC_PIC16F628) || \
      defined(__SDCC_PIC16F628A) || \
      defined(__SDCC_PIC16F648A)
#define __SDCC_PWM_STYLE      1606272

#elif defined(__SDCC_PIC16F685) || \
      defined(__SDCC_PIC16F690)
#define __SDCC_PWM_STYLE      1606852

#elif defined(__SDCC_PIC16F707)
#define __SDCC_PWM_STYLE      1607072

#elif defined(__SDCC_PIC16C717) || \
      defined(__SDCC_PIC16C770) || \
      defined(__SDCC_PIC16C771) || \
      defined(__SDCC_PIC16F716)
#define __SDCC_PWM_STYLE      1607172

#elif defined(__SDCC_PIC16F720) || \
      defined(__SDCC_PIC16F721) || \
      defined(__SDCC_PIC16F913) || \
      defined(__SDCC_PIC16F916)
#define __SDCC_PWM_STYLE      1607202

#elif defined(__SDCC_PIC16F722) || \
      defined(__SDCC_PIC16F722A) || \
      defined(__SDCC_PIC16F723) || \
      defined(__SDCC_PIC16F723A) || \
      defined(__SDCC_PIC16F724) || \
      defined(__SDCC_PIC16F726) || \
      defined(__SDCC_PIC16F727)
#define __SDCC_PWM_STYLE      1607222

#elif defined(__SDCC_PIC16F737) || \
      defined(__SDCC_PIC16F747) || \
      defined(__SDCC_PIC16F767) || \
      defined(__SDCC_PIC16F777)
#define __SDCC_PWM_STYLE      1607372

#elif defined(__SDCC_PIC16F753) || \
      defined(__SDCC_PIC16HV753)
#define __SDCC_PWM_STYLE      1607532

#elif defined(__SDCC_PIC16F785) || \
      defined(__SDCC_PIC16HV785)
#define __SDCC_PWM_STYLE      1607852

#elif defined(__SDCC_PIC16F818) || \
      defined(__SDCC_PIC16F819)
#define __SDCC_PWM_STYLE      1608182

#elif defined(__SDCC_PIC16F882) || \
      defined(__SDCC_PIC16F883) || \
      defined(__SDCC_PIC16F884) || \
      defined(__SDCC_PIC16F886) || \
      defined(__SDCC_PIC16F887)
#define __SDCC_PWM_STYLE      1608822

#elif defined(__SDCC_PIC16F914) || \
      defined(__SDCC_PIC16F917) || \
      defined(__SDCC_PIC16F946)
#define __SDCC_PWM_STYLE      1609142

#elif defined(__SDCC_PIC16F1454) || \
      defined(__SDCC_PIC16F1455)
#define __SDCC_PWM_STYLE      1614542

#elif defined(__SDCC_PIC16F1459)
#define __SDCC_PWM_STYLE      1614592

#elif defined(__SDCC_PIC16F1503) || \
      defined(__SDCC_PIC16F1507) || \
      defined(__SDCC_PIC16F1508) || \
      defined(__SDCC_PIC16F1509)
#define __SDCC_PWM_STYLE      1615032

#elif defined(__SDCC_PIC16F1512) || \
      defined(__SDCC_PIC16F1513) || \
      defined(__SDCC_PIC16F1516) || \
      defined(__SDCC_PIC16F1517) || \
      defined(__SDCC_PIC16F1518) || \
      defined(__SDCC_PIC16F1519)
#define __SDCC_PWM_STYLE      1615122

#elif defined(__SDCC_PIC16F1526) || \
      defined(__SDCC_PIC16F1527)
#define __SDCC_PWM_STYLE      1615262

#elif defined(__SDCC_PIC16F1613)
#define __SDCC_PWM_STYLE      1616132

#elif defined(__SDCC_PIC16F1703)
#define __SDCC_PWM_STYLE      1617032

#elif defined(__SDCC_PIC16F1704) || \
      defined(__SDCC_PIC16F1705)
#define __SDCC_PWM_STYLE      1617042

#elif defined(__SDCC_PIC16F1707)
#define __SDCC_PWM_STYLE      1617072

#elif defined(__SDCC_PIC16F1708)
#define __SDCC_PWM_STYLE      1617082

#elif defined(__SDCC_PIC16F1709)
#define __SDCC_PWM_STYLE      1617092

#elif defined(__SDCC_PIC16F1713) || \
      defined(__SDCC_PIC16F1716) || \
      defined(__SDCC_PIC16F1718)
#define __SDCC_PWM_STYLE      1617132

#elif defined(__SDCC_PIC16F1717) || \
      defined(__SDCC_PIC16F1719)
#define __SDCC_PWM_STYLE      1617172

#elif defined(__SDCC_PIC16F1782) || \
      defined(__SDCC_PIC16F1783)
#define __SDCC_PWM_STYLE      1617822

#elif defined(__SDCC_PIC16F1784) || \
      defined(__SDCC_PIC16F1787)
#define __SDCC_PWM_STYLE      1617842

#elif defined(__SDCC_PIC16F1786)
#define __SDCC_PWM_STYLE      1617862

#elif defined(__SDCC_PIC16F1788)
#define __SDCC_PWM_STYLE      1617882

#elif defined(__SDCC_PIC16F1789)
#define __SDCC_PWM_STYLE      1617892

#elif defined(__SDCC_PIC16F1823)
#define __SDCC_PWM_STYLE      1618232

#elif defined(__SDCC_PIC16F1824) || \
      defined(__SDCC_PIC16F1825)
#define __SDCC_PWM_STYLE      1618242

#elif defined(__SDCC_PIC16F1826)
#define __SDCC_PWM_STYLE      1618262

#elif defined(__SDCC_PIC16F1827) || \
      defined(__SDCC_PIC16F1847)
#define __SDCC_PWM_STYLE      1618272

#elif defined(__SDCC_PIC16F1828) || \
      defined(__SDCC_PIC16F1829)
#define __SDCC_PWM_STYLE      1618282

#elif defined(__SDCC_PIC16F1933) || \
      defined(__SDCC_PIC16F1936) || \
      defined(__SDCC_PIC16F1938)
#define __SDCC_PWM_STYLE      1619332

#elif defined(__SDCC_PIC16F1934) || \
      defined(__SDCC_PIC16F1937) || \
      defined(__SDCC_PIC16F1939)
#define __SDCC_PWM_STYLE      1619342

#elif defined(__SDCC_PIC16F1946) || \
      defined(__SDCC_PIC16F1947)
#define __SDCC_PWM_STYLE      1619462

#else
#warning No PWM style associated with the target device.
#warning Please update your pic14/pic16fam.h manually and/or inform the maintainer.
#endif

/*
 * Define SSP style per device family.
 */
#undef  __SDCC_SSP_STYLE

#if   defined(__SDCC_PIC10F320) || \
      defined(__SDCC_PIC10F322) || \
      defined(__SDCC_PIC12F609) || \
      defined(__SDCC_PIC12F615) || \
      defined(__SDCC_PIC12F617) || \
      defined(__SDCC_PIC12F629) || \
      defined(__SDCC_PIC12F635) || \
      defined(__SDCC_PIC12F675) || \
      defined(__SDCC_PIC12F683) || \
      defined(__SDCC_PIC12F752) || \
      defined(__SDCC_PIC12F1501) || \
      defined(__SDCC_PIC12F1571) || \
      defined(__SDCC_PIC12F1572) || \
      defined(__SDCC_PIC12F1612) || \
      defined(__SDCC_PIC12LF1552) || \
      defined(__SDCC_PIC16C71) || \
      defined(__SDCC_PIC16C432) || \
      defined(__SDCC_PIC16C433) || \
      defined(__SDCC_PIC16C554) || \
      defined(__SDCC_PIC16C557) || \
      defined(__SDCC_PIC16C558) || \
      defined(__SDCC_PIC16C620) || \
      defined(__SDCC_PIC16C620A) || \
      defined(__SDCC_PIC16C621) || \
      defined(__SDCC_PIC16C621A) || \
      defined(__SDCC_PIC16C622) || \
      defined(__SDCC_PIC16C622A) || \
      defined(__SDCC_PIC16C710) || \
      defined(__SDCC_PIC16C711) || \
      defined(__SDCC_PIC16C715) || \
      defined(__SDCC_PIC16C745) || \
      defined(__SDCC_PIC16C765) || \
      defined(__SDCC_PIC16C781) || \
      defined(__SDCC_PIC16C782) || \
      defined(__SDCC_PIC16CR73) || \
      defined(__SDCC_PIC16CR74) || \
      defined(__SDCC_PIC16CR76) || \
      defined(__SDCC_PIC16CR77) || \
      defined(__SDCC_PIC16CR620A) || \
      defined(__SDCC_PIC16F84) || \
      defined(__SDCC_PIC16F84A) || \
      defined(__SDCC_PIC16F610) || \
      defined(__SDCC_PIC16F616) || \
      defined(__SDCC_PIC16F627) || \
      defined(__SDCC_PIC16F627A) || \
      defined(__SDCC_PIC16F628) || \
      defined(__SDCC_PIC16F628A) || \
      defined(__SDCC_PIC16F630) || \
      defined(__SDCC_PIC16F631) || \
      defined(__SDCC_PIC16F636) || \
      defined(__SDCC_PIC16F639) || \
      defined(__SDCC_PIC16F648) || \
      defined(__SDCC_PIC16F648A) || \
      defined(__SDCC_PIC16F676) || \
      defined(__SDCC_PIC16F684) || \
      defined(__SDCC_PIC16F685) || \
      defined(__SDCC_PIC16F688) || \
      defined(__SDCC_PIC16F716) || \
      defined(__SDCC_PIC16F753) || \
      defined(__SDCC_PIC16F785) || \
      defined(__SDCC_PIC16F870) || \
      defined(__SDCC_PIC16F871) || \
      defined(__SDCC_PIC16F1458) || \
      defined(__SDCC_PIC16F1507) || \
      defined(__SDCC_PIC16F1613) || \
      defined(__SDCC_PIC16HV610) || \
      defined(__SDCC_PIC16HV616) || \
      defined(__SDCC_PIC16HV753) || \
      defined(__SDCC_PIC16HV785) || \
      defined(__SDCC_PIC16LF1902) || \
      defined(__SDCC_PIC16LF1903) || \
      defined(__SDCC_PIC16LF1904) || \
      defined(__SDCC_PIC16LF1906) || \
      defined(__SDCC_PIC16LF1907)
#define __SDCC_SSP_STYLE      0

#elif defined(__SDCC_PIC12F1822) || \
      defined(__SDCC_PIC12F1840)
#define __SDCC_SSP_STYLE      1218223

#elif defined(__SDCC_PIC16C62) || \
      defined(__SDCC_PIC16C72)
#define __SDCC_SSP_STYLE      1600623

#elif defined(__SDCC_PIC16C63A) || \
      defined(__SDCC_PIC16C65B)
#define __SDCC_SSP_STYLE      1600633

#elif defined(__SDCC_PIC16F72) || \
      defined(__SDCC_PIC16F73) || \
      defined(__SDCC_PIC16F74) || \
      defined(__SDCC_PIC16F76) || \
      defined(__SDCC_PIC16F77)
#define __SDCC_SSP_STYLE      1600723

#elif defined(__SDCC_PIC16C73B) || \
      defined(__SDCC_PIC16C74B)
#define __SDCC_SSP_STYLE      1600733

#elif defined(__SDCC_PIC16F87) || \
      defined(__SDCC_PIC16F88) || \
      defined(__SDCC_PIC16F818) || \
      defined(__SDCC_PIC16F819)
#define __SDCC_SSP_STYLE      1600873

#elif defined(__SDCC_PIC16F677) || \
      defined(__SDCC_PIC16F687) || \
      defined(__SDCC_PIC16F689) || \
      defined(__SDCC_PIC16F690)
#define __SDCC_SSP_STYLE      1606773

#elif defined(__SDCC_PIC16F707)
#define __SDCC_SSP_STYLE      1607073

#elif defined(__SDCC_PIC16C717) || \
      defined(__SDCC_PIC16C770) || \
      defined(__SDCC_PIC16C771)
#define __SDCC_SSP_STYLE      1607173

#elif defined(__SDCC_PIC16F720) || \
      defined(__SDCC_PIC16F721)
#define __SDCC_SSP_STYLE      1607203

#elif defined(__SDCC_PIC16F722) || \
      defined(__SDCC_PIC16F722A) || \
      defined(__SDCC_PIC16F723) || \
      defined(__SDCC_PIC16F723A) || \
      defined(__SDCC_PIC16F724) || \
      defined(__SDCC_PIC16F726) || \
      defined(__SDCC_PIC16F727)
#define __SDCC_SSP_STYLE      1607223

#elif defined(__SDCC_PIC16F737) || \
      defined(__SDCC_PIC16F747) || \
      defined(__SDCC_PIC16F767) || \
      defined(__SDCC_PIC16F777)
#define __SDCC_SSP_STYLE      1607373

#elif defined(__SDCC_PIC16C773) || \
      defined(__SDCC_PIC16C774)
#define __SDCC_SSP_STYLE      1607733

#elif defined(__SDCC_PIC16F872) || \
      defined(__SDCC_PIC16F873) || \
      defined(__SDCC_PIC16F873A) || \
      defined(__SDCC_PIC16F874) || \
      defined(__SDCC_PIC16F874A) || \
      defined(__SDCC_PIC16F876) || \
      defined(__SDCC_PIC16F876A) || \
      defined(__SDCC_PIC16F877) || \
      defined(__SDCC_PIC16F877A)
#define __SDCC_SSP_STYLE      1608723

#elif defined(__SDCC_PIC16F882) || \
      defined(__SDCC_PIC16F883) || \
      defined(__SDCC_PIC16F884) || \
      defined(__SDCC_PIC16F886) || \
      defined(__SDCC_PIC16F887)
#define __SDCC_SSP_STYLE      1608823

#elif defined(__SDCC_PIC16F913) || \
      defined(__SDCC_PIC16F914) || \
      defined(__SDCC_PIC16F916) || \
      defined(__SDCC_PIC16F917) || \
      defined(__SDCC_PIC16F946)
#define __SDCC_SSP_STYLE      1609133

#elif defined(__SDCC_PIC16C925) || \
      defined(__SDCC_PIC16C926)
#define __SDCC_SSP_STYLE      1609253

#elif defined(__SDCC_PIC16F1454) || \
      defined(__SDCC_PIC16F1455)
#define __SDCC_SSP_STYLE      1614543

#elif defined(__SDCC_PIC16F1459)
#define __SDCC_SSP_STYLE      1614593

#elif defined(__SDCC_PIC16F1503)
#define __SDCC_SSP_STYLE      1615033

#elif defined(__SDCC_PIC16F1508) || \
      defined(__SDCC_PIC16F1509)
#define __SDCC_SSP_STYLE      1615083

#elif defined(__SDCC_PIC16F1512) || \
      defined(__SDCC_PIC16F1513) || \
      defined(__SDCC_PIC16F1516) || \
      defined(__SDCC_PIC16F1517) || \
      defined(__SDCC_PIC16F1518) || \
      defined(__SDCC_PIC16F1519)
#define __SDCC_SSP_STYLE      1615123

#elif defined(__SDCC_PIC16F1526) || \
      defined(__SDCC_PIC16F1527)
#define __SDCC_SSP_STYLE      1615263

#elif defined(__SDCC_PIC16F1703)
#define __SDCC_SSP_STYLE      1617033

#elif defined(__SDCC_PIC16F1704) || \
      defined(__SDCC_PIC16F1705)
#define __SDCC_SSP_STYLE      1617043

#elif defined(__SDCC_PIC16F1707)
#define __SDCC_SSP_STYLE      1617073

#elif defined(__SDCC_PIC16F1708)
#define __SDCC_SSP_STYLE      1617083

#elif defined(__SDCC_PIC16F1709)
#define __SDCC_SSP_STYLE      1617093

#elif defined(__SDCC_PIC16F1713) || \
      defined(__SDCC_PIC16F1716) || \
      defined(__SDCC_PIC16F1718)
#define __SDCC_SSP_STYLE      1617133

#elif defined(__SDCC_PIC16F1717) || \
      defined(__SDCC_PIC16F1719)
#define __SDCC_SSP_STYLE      1617173

#elif defined(__SDCC_PIC16F1782) || \
      defined(__SDCC_PIC16F1783) || \
      defined(__SDCC_PIC16F1784) || \
      defined(__SDCC_PIC16F1786) || \
      defined(__SDCC_PIC16F1787)
#define __SDCC_SSP_STYLE      1617823

#elif defined(__SDCC_PIC16F1788)
#define __SDCC_SSP_STYLE      1617883

#elif defined(__SDCC_PIC16F1789)
#define __SDCC_SSP_STYLE      1617893

#elif defined(__SDCC_PIC16F1823) || \
      defined(__SDCC_PIC16F1824) || \
      defined(__SDCC_PIC16F1825)
#define __SDCC_SSP_STYLE      1618233

#elif defined(__SDCC_PIC16F1826)
#define __SDCC_SSP_STYLE      1618263

#elif defined(__SDCC_PIC16F1827) || \
      defined(__SDCC_PIC16F1847)
#define __SDCC_SSP_STYLE      1618273

#elif defined(__SDCC_PIC16F1828)
#define __SDCC_SSP_STYLE      1618283

#elif defined(__SDCC_PIC16F1829)
#define __SDCC_SSP_STYLE      1618293

#elif defined(__SDCC_PIC16F1933) || \
      defined(__SDCC_PIC16F1934) || \
      defined(__SDCC_PIC16F1936) || \
      defined(__SDCC_PIC16F1937) || \
      defined(__SDCC_PIC16F1938) || \
      defined(__SDCC_PIC16F1939)
#define __SDCC_SSP_STYLE      1619333

#elif defined(__SDCC_PIC16F1946) || \
      defined(__SDCC_PIC16F1947)
#define __SDCC_SSP_STYLE      1619463

#else
#warning No SSP style associated with the target device.
#warning Please update your pic14/pic16fam.h manually and/or inform the maintainer.
#endif

/*
 * Define USART style per device family.
 */
#undef  __SDCC_USART_STYLE

#if   defined(__SDCC_PIC10F320) || \
      defined(__SDCC_PIC10F322) || \
      defined(__SDCC_PIC12F609) || \
      defined(__SDCC_PIC12F615) || \
      defined(__SDCC_PIC12F617) || \
      defined(__SDCC_PIC12F629) || \
      defined(__SDCC_PIC12F635) || \
      defined(__SDCC_PIC12F675) || \
      defined(__SDCC_PIC12F683) || \
      defined(__SDCC_PIC12F752) || \
      defined(__SDCC_PIC12F1501) || \
      defined(__SDCC_PIC12F1571) || \
      defined(__SDCC_PIC12F1612) || \
      defined(__SDCC_PIC12LF1552) || \
      defined(__SDCC_PIC16C62) || \
      defined(__SDCC_PIC16C71) || \
      defined(__SDCC_PIC16C72) || \
      defined(__SDCC_PIC16C432) || \
      defined(__SDCC_PIC16C433) || \
      defined(__SDCC_PIC16C554) || \
      defined(__SDCC_PIC16C557) || \
      defined(__SDCC_PIC16C558) || \
      defined(__SDCC_PIC16C620) || \
      defined(__SDCC_PIC16C620A) || \
      defined(__SDCC_PIC16C621) || \
      defined(__SDCC_PIC16C621A) || \
      defined(__SDCC_PIC16C622) || \
      defined(__SDCC_PIC16C622A) || \
      defined(__SDCC_PIC16C710) || \
      defined(__SDCC_PIC16C711) || \
      defined(__SDCC_PIC16C715) || \
      defined(__SDCC_PIC16C717) || \
      defined(__SDCC_PIC16C770) || \
      defined(__SDCC_PIC16C771) || \
      defined(__SDCC_PIC16C781) || \
      defined(__SDCC_PIC16C782) || \
      defined(__SDCC_PIC16C925) || \
      defined(__SDCC_PIC16C926) || \
      defined(__SDCC_PIC16CR73) || \
      defined(__SDCC_PIC16CR74) || \
      defined(__SDCC_PIC16CR76) || \
      defined(__SDCC_PIC16CR77) || \
      defined(__SDCC_PIC16CR620A) || \
      defined(__SDCC_PIC16F72) || \
      defined(__SDCC_PIC16F84) || \
      defined(__SDCC_PIC16F84A) || \
      defined(__SDCC_PIC16F610) || \
      defined(__SDCC_PIC16F616) || \
      defined(__SDCC_PIC16F630) || \
      defined(__SDCC_PIC16F631) || \
      defined(__SDCC_PIC16F636) || \
      defined(__SDCC_PIC16F639) || \
      defined(__SDCC_PIC16F648) || \
      defined(__SDCC_PIC16F676) || \
      defined(__SDCC_PIC16F677) || \
      defined(__SDCC_PIC16F684) || \
      defined(__SDCC_PIC16F685) || \
      defined(__SDCC_PIC16F716) || \
      defined(__SDCC_PIC16F753) || \
      defined(__SDCC_PIC16F785) || \
      defined(__SDCC_PIC16F818) || \
      defined(__SDCC_PIC16F819) || \
      defined(__SDCC_PIC16F872) || \
      defined(__SDCC_PIC16F1458) || \
      defined(__SDCC_PIC16F1503) || \
      defined(__SDCC_PIC16F1507) || \
      defined(__SDCC_PIC16F1613) || \
      defined(__SDCC_PIC16F1703) || \
      defined(__SDCC_PIC16HV610) || \
      defined(__SDCC_PIC16HV616) || \
      defined(__SDCC_PIC16HV753) || \
      defined(__SDCC_PIC16HV785) || \
      defined(__SDCC_PIC16LF1902) || \
      defined(__SDCC_PIC16LF1903)
#define __SDCC_USART_STYLE    0

#elif defined(__SDCC_PIC12F1572)
#define __SDCC_USART_STYLE    1215724

#elif defined(__SDCC_PIC12F1822) || \
      defined(__SDCC_PIC12F1840)
#define __SDCC_USART_STYLE    1218224

#elif defined(__SDCC_PIC16C63A) || \
      defined(__SDCC_PIC16C65B) || \
      defined(__SDCC_PIC16C73B) || \
      defined(__SDCC_PIC16C74B) || \
      defined(__SDCC_PIC16C745) || \
      defined(__SDCC_PIC16C765) || \
      defined(__SDCC_PIC16F73) || \
      defined(__SDCC_PIC16F74) || \
      defined(__SDCC_PIC16F76) || \
      defined(__SDCC_PIC16F77)
#define __SDCC_USART_STYLE    1600634

#elif defined(__SDCC_PIC16F87) || \
      defined(__SDCC_PIC16F88)
#define __SDCC_USART_STYLE    1600874

#elif defined(__SDCC_PIC16F627) || \
      defined(__SDCC_PIC16F627A) || \
      defined(__SDCC_PIC16F628) || \
      defined(__SDCC_PIC16F628A) || \
      defined(__SDCC_PIC16F648A)
#define __SDCC_USART_STYLE    1606274

#elif defined(__SDCC_PIC16F687) || \
      defined(__SDCC_PIC16F689) || \
      defined(__SDCC_PIC16F690)
#define __SDCC_USART_STYLE    1606874

#elif defined(__SDCC_PIC16F688)
#define __SDCC_USART_STYLE    1606884

#elif defined(__SDCC_PIC16F707)
#define __SDCC_USART_STYLE    1607074

#elif defined(__SDCC_PIC16F720) || \
      defined(__SDCC_PIC16F721)
#define __SDCC_USART_STYLE    1607204

#elif defined(__SDCC_PIC16C773) || \
      defined(__SDCC_PIC16C774) || \
      defined(__SDCC_PIC16F722) || \
      defined(__SDCC_PIC16F722A) || \
      defined(__SDCC_PIC16F723) || \
      defined(__SDCC_PIC16F723A) || \
      defined(__SDCC_PIC16F724) || \
      defined(__SDCC_PIC16F726) || \
      defined(__SDCC_PIC16F727) || \
      defined(__SDCC_PIC16F737) || \
      defined(__SDCC_PIC16F747) || \
      defined(__SDCC_PIC16F767) || \
      defined(__SDCC_PIC16F777) || \
      defined(__SDCC_PIC16F870) || \
      defined(__SDCC_PIC16F871) || \
      defined(__SDCC_PIC16F873) || \
      defined(__SDCC_PIC16F873A) || \
      defined(__SDCC_PIC16F874) || \
      defined(__SDCC_PIC16F874A) || \
      defined(__SDCC_PIC16F876) || \
      defined(__SDCC_PIC16F876A) || \
      defined(__SDCC_PIC16F877) || \
      defined(__SDCC_PIC16F877A) || \
      defined(__SDCC_PIC16F913) || \
      defined(__SDCC_PIC16F914) || \
      defined(__SDCC_PIC16F916) || \
      defined(__SDCC_PIC16F917) || \
      defined(__SDCC_PIC16F946)
#define __SDCC_USART_STYLE    1607734

#elif defined(__SDCC_PIC16F882) || \
      defined(__SDCC_PIC16F883) || \
      defined(__SDCC_PIC16F884) || \
      defined(__SDCC_PIC16F886) || \
      defined(__SDCC_PIC16F887)
#define __SDCC_USART_STYLE    1608824

#elif defined(__SDCC_PIC16F1454) || \
      defined(__SDCC_PIC16F1455)
#define __SDCC_USART_STYLE    1614544

#elif defined(__SDCC_PIC16F1459)
#define __SDCC_USART_STYLE    1614594

#elif defined(__SDCC_PIC16F1508) || \
      defined(__SDCC_PIC16F1509)
#define __SDCC_USART_STYLE    1615084

#elif defined(__SDCC_PIC16F1512) || \
      defined(__SDCC_PIC16F1513) || \
      defined(__SDCC_PIC16F1516) || \
      defined(__SDCC_PIC16F1517) || \
      defined(__SDCC_PIC16F1518) || \
      defined(__SDCC_PIC16F1519)
#define __SDCC_USART_STYLE    1615124

#elif defined(__SDCC_PIC16F1526) || \
      defined(__SDCC_PIC16F1527)
#define __SDCC_USART_STYLE    1615264

#elif defined(__SDCC_PIC16F1704) || \
      defined(__SDCC_PIC16F1705)
#define __SDCC_USART_STYLE    1617044

#elif defined(__SDCC_PIC16F1707)
#define __SDCC_USART_STYLE    1617074

#elif defined(__SDCC_PIC16F1708)
#define __SDCC_USART_STYLE    1617084

#elif defined(__SDCC_PIC16F1709)
#define __SDCC_USART_STYLE    1617094

#elif defined(__SDCC_PIC16F1713) || \
      defined(__SDCC_PIC16F1716) || \
      defined(__SDCC_PIC16F1718)
#define __SDCC_USART_STYLE    1617134

#elif defined(__SDCC_PIC16F1717) || \
      defined(__SDCC_PIC16F1719)
#define __SDCC_USART_STYLE    1617174

#elif defined(__SDCC_PIC16F1782) || \
      defined(__SDCC_PIC16F1783) || \
      defined(__SDCC_PIC16F1784) || \
      defined(__SDCC_PIC16F1786) || \
      defined(__SDCC_PIC16F1787)
#define __SDCC_USART_STYLE    1617824

#elif defined(__SDCC_PIC16F1788)
#define __SDCC_USART_STYLE    1617884

#elif defined(__SDCC_PIC16F1789)
#define __SDCC_USART_STYLE    1617894

#elif defined(__SDCC_PIC16F1823) || \
      defined(__SDCC_PIC16F1824) || \
      defined(__SDCC_PIC16F1825)
#define __SDCC_USART_STYLE    1618234

#elif defined(__SDCC_PIC16F1826) || \
      defined(__SDCC_PIC16F1827) || \
      defined(__SDCC_PIC16F1847)
#define __SDCC_USART_STYLE    1618264

#elif defined(__SDCC_PIC16F1828) || \
      defined(__SDCC_PIC16F1829)
#define __SDCC_USART_STYLE    1618284

#elif defined(__SDCC_PIC16F1933) || \
      defined(__SDCC_PIC16F1934) || \
      defined(__SDCC_PIC16F1936) || \
      defined(__SDCC_PIC16F1937) || \
      defined(__SDCC_PIC16F1938) || \
      defined(__SDCC_PIC16F1939) || \
      defined(__SDCC_PIC16LF1904) || \
      defined(__SDCC_PIC16LF1906) || \
      defined(__SDCC_PIC16LF1907)
#define __SDCC_USART_STYLE    1619334

#elif defined(__SDCC_PIC16F1946) || \
      defined(__SDCC_PIC16F1947)
#define __SDCC_USART_STYLE    1619464

#else
#warning No USART style associated with the target device.
#warning Please update your pic14/pic16fam.h manually and/or inform the maintainer.
#endif

#endif /* !__SDCC_PIC16FAM_H__ */
