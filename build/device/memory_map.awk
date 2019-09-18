/data_section_start_ram/ { data_start = $1 }
/data_section_end_ram/ { data_end = $1 }
/bss_section_start_ram/ { bss_start = $1 }
/bss_section_end_ram/ { bss_end = $1 }
/heap_start/ { heap_start = $1 }
/heap_end/ { heap_end = $1 }
/stack_start/ { stack_start = $1 }
/stack_end/ { stack_end = $1 }

function log_section(name, start, end) {
  printf("%s: 0x%x - 0x%x (0x%x = %d = %dK)\n", name, start, end, end-start, end-start, (end-start)/1024)
}

END {
  log_section("DATA ", data_start, data_end);
  log_section("BSS  ", bss_start, bss_end);
  log_section("HEAP ", heap_start, heap_end);
  log_section("STACK", stack_end, stack_start);
}
