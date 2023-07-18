The flash algorithm ST_STM32F469I_Disco_QSPI.elf assumes that the QSPI flash uses the pin layout below (same as on the ST STM32F469I discovery board):
QUADSPI_BK1_IO0   PF8
QUADSPI_BK1_IO1   PF9
QUADSPI_BK1_IO2   PF7
QUADSPI_BK1_IO3   PF6
QUADSPI_CLK       PF10
QUADSPI_BK1_NCS   PB6

If another pin layout is used for the QSPI interface, please refer to:
https://wiki.segger.com/STM32F4#STM32F469xx_.28Q.29SPI_support


The flash algorithm ST_STM32F412G_Disco_QSPI.elf assumes that the QSPI flash uses the pin layout below (same as on the ST ST_STM32F412G discovery board):
QUADSPI_BK1_IO0   PF8
QUADSPI_BK1_IO1   PF9
QUADSPI_BK1_IO2   PF7
QUADSPI_BK1_IO3   PF6
QUADSPI_CLK       PB2
QUADSPI_BK1_NCS   PG6



The flash algorithm ST_STM32F413H_Disco_QSPI.elf assumes that the QSPI flash uses the pin layout below (same as on the ST ST_STM32F413H discovery board):
QUADSPI_BK1_IO0   PF8
QUADSPI_BK1_IO1   PF9
QUADSPI_BK1_IO2   PE2
QUADSPI_BK1_IO3   PD13
QUADSPI_CLK       PB2
QUADSPI_BK1_NCS   PG6