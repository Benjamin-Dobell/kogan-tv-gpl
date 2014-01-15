/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include "uart16550.h"

void Uart16550Init(uint32 baud, uint8 data, uint8 parity, uint8 stop)
{
    //Enable receiver data available interrupt
}

void Uart16550IntrruptEnable(void)
{
    Uart16550Init(1,2,3,4);
}
