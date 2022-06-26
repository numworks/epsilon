/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Zoltán Vörös
*/

#ifndef _ULAB_IO_
#define _ULAB_IO_

MP_DECLARE_CONST_FUN_OBJ_1(io_load_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(io_loadtxt_obj);
MP_DECLARE_CONST_FUN_OBJ_2(io_save_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(io_savetxt_obj);

#endif