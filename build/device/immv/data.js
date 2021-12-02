let userland_large_symbols = {
  name: "Userland Large symbols",
  start: 0,
  end: 0xFFFF_FFFF,
  zones: [
    {'name': '_ZN13AppsContainer19sharedAppsContainerEv', 'start': 2416011553, 'end': 2416013721}, {'name': '_ZN6Solver3App8Snapshot6unpackEPN6Escher9ContainerE', 'start': 2416255589, 'end': 2416257941}, {'name': '_ZN4I18nL17universalMessagesE.lto_priv.0', 'start': 2416840048, 'end': 2416842412}, {'name': '_ZN8Poincare12Trigonometry28shallowReduceInverseFunctionERNS_10ExpressionENS_14ExpressionNode16ReductionContextE', 'start': 2416620897, 'end': 2416623387}, {'name': '_ZN6Solver13EquationStore17privateExactSolveEPN8Poincare7ContextEb', 'start': 2416251549, 'end': 2416254105}, {'name': 'pow', 'start': 2416327449, 'end': 2416330025}, {'name': '_ZNK5Graph9GraphView8drawRectEP9KDContext6KDRect', 'start': 2416064297, 'end': 2416067033}, {'name': '_ZN8Sequence3App8Snapshot6unpackEPN6Escher9ContainerE', 'start': 2416150993, 'end': 2416153761}, {'name': '_ZN8Poincare12Trigonometry27shallowReduceDirectFunctionERNS_10ExpressionENS_14ExpressionNode16ReductionContextE', 'start': 2416631677, 'end': 2416634563}, {'name': '_ZN8Poincare5ConicC2ENS_10ExpressionEPNS_7ContextEPKcS5_.constprop.0.isra.0', 'start': 2416738329, 'end': 2416741225}, {'name': 'lgamma', 'start': 2416341817, 'end': 2416344773}, {'name': '_ZN10Regression3App8Snapshot6unpackEPN6Escher9ContainerE', 'start': 2416123413, 'end': 2416126525}, {'name': '_ZN8Poincare8Addition13shallowReduceENS_14ExpressionNode16ReductionContextE', 'start': 2416368225, 'end': 2416371545}, {'name': '_ZN4CodeL15catalogChildrenE', 'start': 2416954244, 'end': 2416958084}, {'name': '_ZN10Statistics3App8Snapshot6unpackEPN6Escher9ContainerE', 'start': 2416238765, 'end': 2416242677}, {'name': 'mp_execute_bytecode', 'start': 2416713205, 'end': 2416717117}, {'name': '_ZN5Graph3App8Snapshot6unpackEPN6Escher9ContainerE', 'start': 2416040005, 'end': 2416044153}, {'name': '_ZL9glyphData.lto_priv.1', 'start': 2416779600, 'end': 2416784536}, {'name': '_ZN11Probability3App8Snapshot6unpackEPN6Escher9ContainerE', 'start': 2416089589, 'end': 2416094777}, {'name': '_ZN8Poincare14Multiplication20privateShallowReduceENS_14ExpressionNode16ReductionContextEb', 'start': 2416526129, 'end': 2416532073}, {'name': '_ZL9glyphData.lto_priv.0', 'start': 2416772876, 'end': 2416779600}, {'name': '_ZN8Poincare10Polynomial20CubicPolynomialRootsENS_10ExpressionES1_S1_S1_PS1_S2_S2_S2_PNS_7ContextENS_11Preferences13ComplexFormatENS5_9AngleUnitEPb', 'start': 2416547329, 'end': 2416555821}, {'name': '_ZN8Poincare5Power13shallowReduceENS_14ExpressionNode16ReductionContextE', 'start': 2416568901, 'end': 2416582073}, {'name': '_ZN4I18nL8messagesE.lto_priv.0', 'start': 2416912128, 'end': 2416938644}, {'name': '_ZN3Ion17staticStorageAreaE', 'start': 536986404, 'end': 537019192}, {'name': '_ZZN8Poincare4InitEvE4pool', 'start': 537019200, 'end': 537062896}, {'name': '_ZZN13AppsContainer19sharedAppsContainerEvE20appsContainerStorage', 'start': 536871480, 'end': 536985688}]
}

let stm32f730 = {
  name: "STM32F730",
  start: 0x0000_0000,
  end: 0xFFFF_FFFF,
  zones: [
    {
      name: "ITCM RAM",
      start: 0x0000_0000,
      end: 0x0000_3FFF
    },
    {
      name: "System memory",
      start: 0x0010_0000,
      end:0x0010_EDBF
    },
    {
      name: "Flash memory on ITCM interface",
      start: 0x0020_0000,
      end: 0x0020_FFFF
    },
    {
      name: "Flash memory on AXIM interface",
      start: 0x0800_0000,
      end: 0x0800_FFFF
    },
    {
      name: "Option Bytes",
      start: 0x1FFF_0000,
      end: 0x1FFF_001F
    },
    {
      name: "DTCM",
      start: 0x2000_0000,
      end: 0x2000_FFFF
    },
    {
      name: "SRAM1",
      start: 0x2001_0000,
      end: 0x2003_BFFF
    },
    {
      name: "SRAM2",
      start: 0x2003_C000,
      end: 0x2003_FFFF
    },
    {
      name: "External Flash",
      start: 0x90000000,
      end: 0x9080_0000
    }
  ]
}

let userland = {
  name: "Userland",
  start: 0x2000_0000,
  end: 0x903E_FFFF,
  zones: [
    {
      name: ".data",
      start: 0x2000_0000,
      end: 0x2000_005F
    },
    {
      name: ".bss",
      start: 0x2000_0060,
      end: 0x2002_F4CB
    },
    {
      name: ".heap",
      start: 0x2002_F4CC,
      end: 0x2003_6FFF
    },
    {
      name: ".process_stack",
      start: 0x2003_7000,
      end: 0x2003_EFF8
    },
    {
      name: ".userland_header",
      start: 0x9001_0000,
      end: 0x9001_001F
    },
    {
      name: ".isr_vector_table",
      start: 0x9001_0020,
      end: 0x9001_002F
    },
    {
      name: ".text",
      start: 0x9001_0030,
      end: 0x900C_E483
    },
    {
      name: ".dfu_entry_point",
      start: 0x900C_E484,
      end: 0x900C_EAC7
    },
    {
      name: ".rodata",
      start: 0x900C_EAC8,
      end: 0x9011_D7A2
    },
    {
      name: ".init_array",
      start: 0x9011_D7A3,
      end: 0x9011_D7A4
    },

    {
      name: ".signature",
      start: 0x9011_D804,
      end: 0x9011_D843
    },
    {
      name: ".storage_flash",
      start: 0x9011_D844,
      end: 0x903E_FFFF
    }
  ]
}

let kernel = {
  name: "Kernel",
  start: 0x2003_7000,
  end: 0x903F_FFFF,
  zones: [
    {
      name: ".process_stack",
      start: 0x2003_7000,
      end: 0x2003_EFF7
    },
    {
      name: ".main_stack",
      start: 0x2003_F000,
      end: 0x2003_F3F7
    },
    {
      name: ".isr_vector_table",
      start: 0x2003_F400,
      end: 0x2003_F5C3
    },
    {
      name: ".data",
      start: 0x2003_F5C8,
      end: 0x2003_F720
    },
    {
      name: ".bss",
      start: 0x2003_F728,
      end: 0x2003_FCA7
    },
    {
      name: ".canary",
      start: 0x2003_FCA8,
      end: 0x2003_FFFF
    },
    {
      name: ".signed_payload_prefix",
      start: 0x9000_0000,
      end: 0x9000_0007
    },
    {
      name: ".kernel_header",
      start: 0x9000_0008,
      end: 0x9000_001F
    },
    {
      name: ".text",
      start: 0x9000_01E4,
      end: 0x9000_41D7
    },
    {
      name: ".rodata",
      start: 0x9000_41D8,
      end: 0x9000_7AC3
    },
    {
      name: ".fill",
      start: 0x9000_7C1D,
      end: 0x9000_FFFF
    },
    /*
    {
      name: ".persisting_bytes_buffer",
      start: 0x903F_0000,
      end: 0x903F_FFFF
    }
    */
  ]
}


let kernelA = {
    "name": "kernel.A.elf",
    "start": 537096192,
    "end": 2420113408,
    "zones": [
        {
            "name": ".signed_payload_prefix",
            "start": 2415919104,
            "end": 2415919112
        },
        {
            "name": ".kernel_header",
            "start": 2415919112,
            "end": 2415919136
        },
        {
            "name": ".isr_vector_table",
            "start": 537129984,
            "end": 537130436
        },
        {
            "name": ".text",
            "start": 2415919588,
            "end": 2415935944
        },
        {
            "name": ".rodata",
            "start": 2415935944,
            "end": 2415950516
        },
        {
            "name": ".data",
            "start": 537130440,
            "end": 537130785
        },
        {
            "name": ".fill",
            "start": 2415950861,
            "end": 2415984640
        },
        {
            "name": ".persisting_bytes_buffer",
            "start": 2420047872,
            "end": 2420113408
        },
        {
            "name": ".bss",
            "start": 537130792,
            "end": 537131864
        },
        {
            "name": ".canary",
            "start": 537131864,
            "end": 537133056
        },
        {
            "name": ".process_stack",
            "start": 537096192,
            "end": 537128952
        },
        {
            "name": ".main_stack",
            "start": 537128960,
            "end": 537129976
        }
    ]
}




let kernelB = {
    "name": "kernel.B.elf",
    "start": 537096192,
    "end": 2424307712,
    "zones": [
        {
            "name": ".signed_payload_prefix",
            "start": 2420113408,
            "end": 2420113416
        },
        {
            "name": ".kernel_header",
            "start": 2420113416,
            "end": 2420113440
        },
        {
            "name": ".isr_vector_table",
            "start": 537129984,
            "end": 537130436
        },
        {
            "name": ".text",
            "start": 2420113892,
            "end": 2420130248
        },
        {
            "name": ".rodata",
            "start": 2420130248,
            "end": 2420144820
        },
        {
            "name": ".data",
            "start": 537130440,
            "end": 537130785
        },
        {
            "name": ".fill",
            "start": 2420145165,
            "end": 2420178944
        },
        {
            "name": ".persisting_bytes_buffer",
            "start": 2424242176,
            "end": 2424307712
        },
        {
            "name": ".bss",
            "start": 537130792,
            "end": 537131864
        },
        {
            "name": ".canary",
            "start": 537131864,
            "end": 537133056
        },
        {
            "name": ".process_stack",
            "start": 537096192,
            "end": 537128952
        },
        {
            "name": ".main_stack",
            "start": 537128960,
            "end": 537129976
        }
    ]
}



let userland_ram = {
  name: "Userland RAM",
  start: 0x2000_0000,
  end: 0x2003_EFF8,
  zones: [
    {
      name: ".data",
      start: 0x2000_0000,
      end: 0x2000_005F
    },
    {
      name: ".bss",
      start: 0x2000_0060,
      end: 0x2002_F4CB
    },
    {
      name: ".heap",
      start: 0x2002_F4CC,
      end: 0x2003_6FFF
    },
    {
      name: ".process_stack",
      start: 0x2003_7000,
      end: 0x2003_EFF8
    }
  ]
}

let userland_flash = {
  name: "Userland Flash",
  start: 0x9001_0000,
  end: 0x903E_FFFF,
  zones: [
    {
      name: ".userland_header",
      start: 0x9001_0000,
      end: 0x9001_001F
    },
    {
      name: ".isr_vector_table",
      start: 0x9001_0020,
      end: 0x9001_002F
    },
    {
      name: ".text",
      start: 0x9001_0030,
      end: 0x900C_E483
    },
    {
      name: ".dfu_entry_point",
      start: 0x900C_E484,
      end: 0x900C_EAC7
    },
    {
      name: ".rodata",
      start: 0x900C_EAC8,
      end: 0x9011_D7A2
    },
    {
      name: ".init_array",
      start: 0x9011_D7A3,
      end: 0x9011_D7A4
    },

    {
      name: ".signature",
      start: 0x9011_D804,
      end: 0x9011_D843
    },
    {
      name: ".storage_flash",
      start: 0x9011_D844,
      end: 0x903E_FFFF
    }
  ]
}

let data = [stm32f730, kernelA, userland_flash, userland_large_symbols]
