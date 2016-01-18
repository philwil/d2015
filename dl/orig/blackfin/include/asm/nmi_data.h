/*
 * U-boot - nmi_data.h Declarations for shared u-boot / linux data
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef	__ASM_NMI_DATA_H
#define __ASM_NMI_DATA_H

typedef struct nmi_data {
  void *myloc;       // check for location
  void *global;      // the gd structure is here !!
  int nmi_count;     // count of nmi irqs seen
  void *nmi_evt;     // ??
  void *lat;         // lats data pointer
  int num_lats;      // num of nmi lats
  int l1_data_size;  // size of l1 data used by nmi
  int l1_text_size;  // size of l1 text used by nmi
  void *zmsg;        // msg data pointer
  int num_msgs;      // num msgs from nmi system
  int nmi_trig;      // used to hanshake setup
  void *evt2;        //  when this is non zero then switch nmi vector to this
  void *nmi_run;     // address of the u-boot nmi control routine
  void * (*nmi_fcn)(void*data);     // address of u-boot function to run under linux nmi
  void *nmi_fcn_data;// address of u-boot function data (may not use this)
  void *zmsg_pairs;  // address of zmsg_pairs
  int num_pairs;     // num pairs from nmi system
  void *zmem;        // address of zmem areas
  int num_mem;       // num zmem areas
  void *zmap;        // address of zmap areas
  int num_map;       // num zmap areas
  void *(*run_command)(void * data, int val); // u-boot run command !!! 
  int * nmi_stack;     // start of nmi stack
} nmi_t;

#endif
