References: https://wiki.dfrobot.com/sen0644/
Factory Reset trước khi trả sản phẩm

#Giai đoạn 1: Sử dụng code mẫu của hãng giao tiếp với Arduino thông qua module RS485-TTL.

#Giai đoạn 2: Thử giao tiếp với cảm biến bằng frame mã HEX thông qua USB to RS485 (USB-TTL & RS485-TTL)

Thử đổi địa chỉ của cảm biến (đã có lệnh Factory Reset), nhưng phải ghi chú
- Cần phải tính lại CRC code mỗi khi đổi địa chỉ của thiết bị. Công cụ tính CRC: https://crccalc.com/?crc=123456789&method=&datatype=ascii&outtype=hex
- CRC tại địa chỉ 0x1 là 0xCB65, viết thành frame: 01 03 00 02 00 02 65 CB.
- CRC tại địa chỉ 0x02 là 0xF865, viết thành frame: 02 03 00 02 00 02 65 F8

Thử đổi baudrate của cảm biến

#Giai đoạn 3: Sử dụng USB-RS485 để cấu hình cảm biến. Sửa code mẫu của hãng (địa chỉ và bauđrate) và đọc tín hiệu đã qua xử lý (Lux).

#Giai đoạn 4: Viết chương trình giao tiếp với Arduino Uno quét địa chỉ và baudrate của cảm biến bằng cách truyền Frame HEX, nhận Frame HEX và in lên màn hình (tham khảo SEN0644_Read-HexData). 

#Giai đoạn 5: Viết chương trình giao tiếp với Arduino Uno cấu hình cảm biến: thay đổi địa chỉ và baudrate bằng cách truyền Frame HEX, nhận Frame HEX và in lên màn hình. In thêm nhắc nhở ngắt nguồn và kết nối lại.

#Giai đoạn 6: Sửa chương trình giao tiếp với Arduino Uno quét địa chỉ và baudrate của cảm biến, in nội dung lên Serial Monitor: Địa chỉ và Baudrate. 


Giai đoạn 7: Sửa chương trình giao tiếp với Arduino Uno cấu hình cảm biến: thay đổi địa chỉ và baudrate bằng cách truyền Frame HEX, in kết quả địa chỉ và baudrate mới lên serial monitor. In thêm nhắc nhở ngắt nguồn và kết nối lại.
