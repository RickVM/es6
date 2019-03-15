//   char* valueData, retv;
//   const char* memAddr;
//   ssize_t addressSize;
//   const char* delimiter = " ";

//   used_buffer_size =
//       count > sysfs_max_data_size
//     ? sysfs_max_data_size
//     : count; /* handle MIN(used_buffer_size, count) bytes */

//   // Perhaps use this or vssanf sscanf
//   printk(KERN_INFO "sysfile_write (/sys/kernel/%s/%s) called, buffer: "
//        "%s, count: %u\n", sysfs_dir, sysfs_file, buffer, count);

//   strcpy(valueData, buffer);
//   memAddr = valueData;
//   retv = strsep(&valueData, delimiter); // count(buffer size) > max data size we might have a problem here.
//   printk("Retv: %s, memAddress %s,value: %s", retv, memAddr, valueData);
//   strcpy(memAddress, memAddr);
//   addressSize = strlen(memAddr);

//   memcpy(sysfs_buffer, buffer, used_buffer_size-addressSize);
//   sysfs_buffer[used_buffer_size] = '\0'; /* this is correct, the buffer
//       is declared to be sysfs_max_data_size+1 bytes! */

//   return used_buffer_size;