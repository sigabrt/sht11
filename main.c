#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <math.h>

#define NOP() __asm__ __volatile__("nop")

#define SCK (DDD2)
#define DATA (DDD3)

#define SCK_LO() (DDRD |= (1 << SCK))
#define SCK_HI() (DDRD &= ~(1 << SCK))

#define DATA_LO() (DDRD |= (1 << DATA))
#define DATA_HI() (DDRD &= ~(1 << DATA))

typedef enum
{
	SHT11_MEASURE_TEMP = 0x3, // 00000011
	SHT11_MEASURE_HUMIDITY = 0x5, // 00000101
} sht11_cmd_t;

void uart_init(void);
void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);

void sht11_trans_start(void);
void sht11_send(unsigned char byte);
unsigned char sht11_send_cmd(sht11_cmd_t cmd);
unsigned char sht11_recv(unsigned char ack);
void sht11_reset(void);

float calc_temp(int input);
float calc_humidity(int input);

int main(void)
{
	// Configure LED pin (pin 13 on arduino uno)
	DDRB |= (1 << DDB5);

	// Configure UART
	FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
	FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
	uart_init();
	stdout = &uart_output;
	stdin = &uart_input;

	_delay_ms(20);

	unsigned char error;
	unsigned int temp, humid;
	while(1)
	{
		PORTB &= ~(1 << PORTB5); // Turn LED off

		error = sht11_send_cmd(SHT11_MEASURE_TEMP);
		if(error)
		{
			sht11_reset();
			continue;
		}
		temp = sht11_recv(1) << 8;
		temp |= sht11_recv(1);
		sht11_recv(0); // Checksum

		error = sht11_send_cmd(SHT11_MEASURE_HUMIDITY);
		if(error)
		{
			sht11_reset();
			continue;
		}
		humid = sht11_recv(1) << 8;
		humid |= sht11_recv(1);
		sht11_recv(0); // Checksum

		float realtemp = calc_temp(temp);
		float realhumid = calc_humidity(humid);
		printf("Temp: %.2f F\tHumidity: %.2f%%\n", realtemp, realhumid);

		PORTB |= (1 << PORTB5); // Turn LED on

		// The sensor needs to be off for a while
		// so it doesn't heat up and throw off the
		// measurements
		_delay_ms(800);
	}
}

void uart_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

#if USE_2X
	UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);	// 8-bit data
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);	// Enable RX and TX
}

void uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
	{
		uart_putchar('\r', stream);
	}
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

char uart_getchar(FILE *stream)
{
	loop_until_bit_is_set(UCSR0A, RXC0); // Wait until data exists.
	return UDR0;
}

void sht11_trans_start(void)
{
	SCK_LO();
	DATA_HI();
	NOP();
	SCK_HI();
	NOP();
	DATA_LO();
	NOP();
	SCK_LO();
	NOP();
	NOP();
	NOP();
	SCK_HI();
	NOP();
	DATA_HI();
	NOP();
	SCK_LO();
}

void sht11_send(unsigned char byte)
{
	int i;
	for(i = 0x80; i > 0; i /= 2)
	{
		if(byte & i)
		{
			DATA_HI();
		}
		else
		{
			DATA_LO();
		}
		SCK_HI();
		NOP();
		NOP();
		NOP();
		SCK_LO();
	}
	DATA_HI(); // Release data pin
}

// Blocks until measurement complete or error (non-zero output)
unsigned char sht11_send_cmd(sht11_cmd_t cmd)
{
	unsigned char error;
	unsigned int i;

	sht11_trans_start();
	sht11_send(cmd);

	// Wait for ACK
	SCK_HI();
	error = (PIND & (1 << PIND3));
	SCK_LO();

	// Wait for measurement
	for(i = 0; i < 2000; i++)
	{
		if(!(PIND & (1 << PIND3)))
		{
			break;
		}
		_delay_ms(1);
	}
	if(PIND & (1 << PIND3))
	{
		error = 1;
	}
	return error;
}

unsigned char sht11_recv(unsigned char ack)
{
	unsigned char i, ret = 0;

	DATA_HI();
	for(i = 0x80; i > 0; i /= 2)
	{
		SCK_HI();
		if(PIND & (1 << PIND3))
		{
			ret |= i;
		}
		SCK_LO();
	}
	if(ack)
	{
		DATA_LO();
	}
	else
	{
		DATA_HI();
	}
	SCK_HI();
	NOP();
	NOP();
	NOP();
	SCK_LO();

	DATA_HI(); // Release data pin

	return ret;
}

void sht11_reset(void)
{
	unsigned char i;

	DATA_HI();
	SCK_LO();
	for(i = 0; i < 9; i++)
	{
		SCK_HI();
		SCK_LO();
	}
	sht11_trans_start();
}

// Returns temperature in degrees fahrenheit
float calc_temp(int input)
{
	return 0.018 * input - 40;
}

// Returns relative humidity in %
float calc_humidity(int input)
{
	return -4 + 0.0405 * input + -2.8e-6 * pow(input, 2);
}

