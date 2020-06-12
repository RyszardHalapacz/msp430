

#include <msp430.h>
/********************************************************************************************************/
#define RED BIT6                        // Red LED -> P1.6
#define GRN BIT0                        // Green LED -> P1.0
#define SW BIT3                         // Switch -> P1.3

#define GRN_RGB BIT1                        // Zdefiniujemy bit1 jako kolor zielony diody RGB
#define RED_RGB BIT3                        // Zdefiniujemy bit3 jako kolor czerwony diody RGB
#define BLU_RGB BIT5                        // Zdefiniujemy bit5 jako kolor niebieski diody RGB

/********************************************************************************************************/

volatile int room_time=10;
int time[]={0,0};
unsigned int tab[3]      =   {0,0,0};
unsigned int rgb [8][3]  =  {{0,0,0},
                             {0,0,1},
                             {0,1,0},
                             {0,1,1},
                             {1,0,0},
                             {1,0,1},
                             {1,1,0},
                             {1,1,1}};

   char times[6] = {'0','0','0','0','0','0'};
   char *wsk = &times[5];
   char char_of_int;
   int  number;

/********************************************************************************************************/


/********************************************************************************************************/

void print(char *text);                // funkcja wysy³aj¹ca wsking na wyjœcie Tx znak po znaku
void print_num(int num);


void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;           // Stop Watchdog


    TACCTL0 |= CCIE;
    TACTL |= TASSEL_2  + TACLR + ID_3;
    TA0CCR0 = 12500;

    TA1CCTL0 |= CCIE;
    TA1CTL |= TASSEL_2  + TACLR + ID_3;
    TA1CCR0 = 12500;



    //Konfiguracja I/O
    P1DIR |= RED;
    P1OUT &= ~RED;

    P1DIR |= GRN;

    P2DIR |= (GRN_RGB + RED_RGB + BLU_RGB);
    P2DIR &= ~GRN_RGB;
    P2DIR &= ~RED_RGB;
    P2DIR &= ~BLU_RGB;



    P1OUT &= ~GRN;
    P1DIR &= ~SW;
    P1REN |= SW;
    P1OUT |= SW;

    // Konfiguracja zegara dla UART
    if (CALBC1_1MHZ==0xFF)
    {
        while(1);
    }
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    // Ustawienie pinów P1.1 i P1.2 jako Rx i Tx
    P1SEL = BIT1 + BIT2 ;
    P1SEL2 = BIT1 + BIT2;

    // Konfiguracja parametrów transmisji UART

    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_1;
    UCA0CTL1 &= ~UCSWRST;
    // przerwanie uruchamiane poprzez odebranie sygna³u na pinie Rx
    IE2 |= UCA0RXIE;

    //konfiguracja przerwania  przycisku
    P1IES &= ~SW;
    P1IE |= SW;
    print ("Technika Mikroprocesorowa Wyklad\r\n Ryszard H.\r\n");
    __bis_SR_register(LPM0_bits + GIE);
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){          // Przerwanie wykonane przez naciœniêcie przycisku

    P2DIR &= ~GRN_RGB;
    P2DIR &= ~RED_RGB;
    P2DIR &= ~BLU_RGB;

    P1OUT &= ~GRN;
    P1OUT &= ~RED;

    tab[0] = 0;
    tab[1] = 0;
    tab[2] = 0;
    time[0] = 0;
    time[1] = 0;
    P1IFG &= ~SW;                       // Czyszczenie flagi przerwania na Przycisku
}

#pragma vector=USCIAB0RX_VECTOR         // Przerwanie uruchamiane po odebraniu znaku
__interrupt void USCI0RX_ISR(void)
{
    //  UCA0RXBUF rejewsk odbioru
    // UCA0TXBUF rejewsk wysylania
    while (!(IFG2&UCA0TXIFG));
    {
        switch(UCA0RXBUF)
                {
                    case 'r':
                    {
                        tab[0]+=1;
                        P1OUT ^= RED;
                        print("Status pokoju czeronwego: ");
                        if((tab[0])%2)
                         {
                            TACTL |= MC_1;
                            print("Pokoj czerwony swieci \r\n");
                         }
                        else
                        {
                            TACTL &= ~MC_3;
                            print("Pokoj czerwony nie swieci \r\n");
                        }
                        break;
                    }
                    case 'g':
                    {
                        tab[1]+=1;
                        P1OUT ^=GRN;
                        print("Status pokoju zielonego: ");
                        if((tab[1])%2)
                        {
                            TA1CTL |= MC_1;
                            print("Pokoj zielony swieci \r\n");

                        }
                        else
                        {
                            TA1CTL &= ~MC_3;
                            print("Pokoj zielony nie swieci \r\n");

                        }
                       break;
                    }
                    case 'u':
                    {
                         tab[2]+=1;
                         if ((tab[2]==8)) tab[2]=0;
                         if((rgb[tab[2]][0])==1)   P2DIR |= GRN_RGB; else     P2DIR &= ~GRN_RGB;
                         if((rgb[tab[2]][1])==1)   P2DIR |= RED_RGB; else     P2DIR &= ~RED_RGB;
                         if((rgb[tab[2]][2])==1)   P2DIR |= BLU_RGB; else     P2DIR &= ~BLU_RGB;
                        break;
                    }
                    case 'd':
                    {
                        tab[2]+=1;
                        if ((tab[2]==8))  tab[2]=0;
                        if((rgb[tab[2]][0])==1)   P2DIR |= GRN_RGB; else     P2DIR &= ~GRN_RGB;
                        if((rgb[tab[2]][1])==1)   P2DIR |= RED_RGB; else     P2DIR &= ~RED_RGB;
                        if((rgb[tab[2]][2])==1)   P2DIR |= BLU_RGB; else     P2DIR &= ~BLU_RGB;
                        break;
                    }
                    case 's':
                    {
                        if((tab[0])%2) print("Pokoj czerwony swieci \r\n");
                        else print("Pokoj czerwony nie swieci \r\n");
                        print_num(time[0]);

                        if((tab[1])%2) print("Pokoj zielony swieci \r\n");
                        else print("Pokoj zielony nie swieci \r\n");
                        print_num(time[1]);

                        if((tab[2]<=7) && (tab[2]>0) ) print("Pokój taneczny swieci") ;

                        else print("Pokoj taneczny nie swieci \r\n") ;
                        break;
                    }
                }
    }
}




#pragma vector = TIMER0_A0_VECTOR       // Przerwanie po odliczeniu czasu timera 0
__interrupt void CCR0_ISR(void)
{
    time[0]+=room_time;
}
#pragma vector = TIMER1_A0_VECTOR       // Przerwanie po odliczeniu czasu timera 1
__interrupt void CCR1_ISR(void)
{
    time[1]+=room_time;
}

/***************************************************************************************************/
void print(char *text){                 // funkcja wysy³aj¹ca wsking na wyjœcie Tx znak po znaku
    unsigned int i = 0;
    while(text[i] != '\0')
    {
        while (!(IFG2&UCA0TXIFG));
        UCA0TXBUF = text[i];
        i++;
    }
}

void print_num(int num)
{
    *wsk = '\0';
    do
    {
        number = num;
        num /= 10;
         char_of_int = (number - 10 * num) + '0';
        *--wsk = char_of_int;
    } while(num);
    print("Czas swiecenia pokoju:");
    print(wsk);
    print("ms \r\n");
}



