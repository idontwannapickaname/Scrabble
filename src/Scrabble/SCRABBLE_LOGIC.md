# Scrabble Game Logic - Tiếng Việt

## Tổng quan
Game Scrabble đã được cải thiện với logic đầy đủ để kiểm tra từ điển tiếng Anh và tính điểm chính xác **chỉ cho lượt chơi hiện tại** khi ấn nút SUBMIT.

## Các tính năng chính

### 1. Hệ thống từ điển
- **Tự động load từ điển**: Game sẽ cố gắng load file `assets/dictionary/dictionary.txt`
- **Fallback dictionary**: Nếu không tìm thấy file, sẽ sử dụng danh sách từ cơ bản tiếng Anh
- **Kiểm tra từ hợp lệ**: Chỉ những từ được tạo ra từ chữ cái mới mới được tính điểm

### 2. Logic tính điểm **CHỈ CHO LƯỢT HIỆN TẠI**
- **Điểm chữ cái**: Mỗi chữ cái có giá trị riêng (A=1, B=3, C=3, D=2, E=1, F=4, G=2, H=4, I=1, J=8, K=5, L=1, M=3, N=1, O=1, P=3, Q=10, R=1, S=1, T=1, U=1, V=4, W=4, X=8, Y=4, Z=10)
- **Hệ số nhân chữ cái**: 
  - Double Letter (DL): Nhân đôi điểm chữ cái
  - Triple Letter (TL): Nhân ba điểm chữ cái
- **Hệ số nhân từ**:
  - Double Word (DW): Nhân đôi điểm từ
  - Triple Word (TW): Nhân ba điểm từ
  - Center Star: Nhân đôi điểm từ
- **Chỉ tính điểm cho từ mới**: Chỉ những từ chứa ít nhất 1 chữ cái mới được đặt trong lượt hiện tại

### 3. Quy trình chơi cải tiến
1. **Đặt chữ cái**: Click vào chữ cái, rồi click vào vị trí trên bàn cờ
2. **Theo dõi chữ cái mới**: Game tự động ghi nhớ vị trí chữ cái mới đặt
3. **Kiểm tra**: Ấn nút SUBMIT để kiểm tra từ hợp lệ
4. **Tính điểm**: Chỉ tính điểm cho từ được tạo ra từ chữ cái mới
5. **Chuyển lượt**: Người chơi mới nhận 7 chữ cái mới

**Các tùy chọn bổ sung:**
- **UNDO**: Nếu đặt sai chữ cái, có thể đưa về hàng chờ
- **SKIP**: Nếu không muốn đặt chữ cái, có thể bỏ lượt

### 4. Kiểm tra từ hợp lệ **CHỈ CHO LƯỢT HIỆN TẠI**
- **Tìm từ mới**: Chỉ tìm những từ chứa chữ cái mới được đặt
- **Kiểm tra từ điển**: Mỗi từ phải có trong từ điển tiếng Anh
- **Từ phải có độ dài >= 2**: Chỉ tính điểm cho từ có ít nhất 2 chữ cái
- **Phải chứa chữ cái mới**: Từ chỉ được tính điểm nếu chứa ít nhất 1 chữ cái mới

### 5. Ví dụ logic tính điểm
```
Lượt 1 - Player 1 đặt chữ cái "HELLO" ở giữa bàn cờ:
- Từ "HELLO" được tạo ra
- Chỉ tính điểm cho từ "HELLO" (không tính từ cũ nếu có)
- Điểm = (H=4 + E=1 + L=1 + L=1 + O=1) × Center Star(2) = 8 × 2 = 16 điểm

Lượt 2 - Player 2 đặt chữ cái "WORLD" nối với "HELLO":
- Từ "HELLOWORLD" được tạo ra
- Chỉ tính điểm cho từ "WORLD" (không tính lại "HELLO")
- Điểm = (W=4 + O=1 + R=1 + L=1 + D=2) = 9 điểm
```

### 6. Cấu trúc bàn cờ
```
TW  DL  DL  DL  TW  DL  DL  DL  DL  DL  DL  DL  DL  DL  TW
DL  DW  DL  DL  DL  DL  TL  DL  DL  TL  DL  DL  DL  DW  DL
DL  DL  DW  DL  DL  DL  DL  DL  DL  DL  DL  DL  DW  DL  DL
DL  DL  DL  DW  DL  DL  DL  DL  DL  DL  DL  DW  DL  DL  DL
DL  DL  DL  DL  DW  DL  DL  DL  DL  DL  DW  DL  DL  DL  DL
DL  DL  TL  DL  DL  TL  DL  DL  DL  TL  DL  DL  TL  DL  DL
DL  TL  DL  DL  DL  DL  TL  DL  TL  DL  DL  DL  DL  TL  DL
TL  DL  DL  DL  DL  DL  DL  ★   DL  DL  DL  DL  DL  DL  TL
DL  TL  DL  DL  DL  DL  TL  DL  TL  DL  DL  DL  DL  TL  DL
DL  DL  TL  DL  DL  TL  DL  DL  DL  TL  DL  DL  TL  DL  DL
DL  DL  DL  DL  DW  DL  DL  DL  DL  DL  DW  DL  DL  DL  DL
DL  DL  DL  DW  DL  DL  DL  DL  DL  DL  DL  DW  DL  DL  DL
DL  DL  DW  DL  DL  DL  DL  DL  DL  DL  DL  DL  DW  DL  DL
DL  DW  DL  DL  DL  DL  TL  DL  DL  TL  DL  DL  DL  DW  DL
TW  DL  DL  DL  TW  DL  DL  DL  DL  DL  DL  DL  DL  DL  TW
```

**Chú thích:**
- TW = Triple Word (Nhân ba từ)
- DW = Double Word (Nhân đôi từ)  
- TL = Triple Letter (Nhân ba chữ cái)
- DL = Double Letter (Nhân đôi chữ cái)
- ★ = Center Star (Điểm bắt đầu, nhân đôi từ)

## Cách sử dụng

### Biên dịch và chạy
```bash
# Biên dịch
g++ -o Scrabble main.cpp GameWindow.cpp Menu.cpp ScrabbleGame.cpp -lSDL2 -lSDL2_ttf -lSDL2_image

# Chạy game
./Scrabble
```

### Điều khiển
- **Chuột**: Click để chọn chữ cái và đặt lên bàn cờ
- **Phím Space**: Chuyển đổi chế độ đặt chữ cái
- **Phím Enter**: Đặt chữ cái thủ công
- **Nút SUBMIT**: Kiểm tra từ mới và tính điểm
- **Nút UNDO**: Đưa chữ cái cuối cùng về hàng chờ
- **Nút SKIP**: Chuyển lượt mà không tính điểm
- **Nút QUIT**: Thoát game

## Lưu ý kỹ thuật

### Cải thiện đã thực hiện
1. **Tích hợp từ điển**: Tự động load và kiểm tra từ hợp lệ
2. **Logic tính điểm chính xác**: Hỗ trợ tất cả hệ số nhân
3. **Kiểm tra từ mới**: Chỉ tìm và validate từ chứa chữ cái mới
4. **Theo dõi lượt chơi**: Ghi nhớ chữ cái mới đặt trong mỗi lượt
5. **Tính điểm công bằng**: Chỉ tính điểm cho từ mới, không tính lại từ cũ

### Cần cải thiện thêm
1. **Giao diện thông báo**: Hiển thị từ mới tìm được và điểm số
2. **Lưu trạng thái**: Lưu game để có thể tiếp tục sau
3. **Animation**: Hiệu ứng khi đặt chữ cái và tính điểm

## Kết luận
Game Scrabble hiện tại đã có logic **HOÀN CHỈNH VÀ CÔNG BẰNG** với:
- ✅ Hệ thống từ điển tiếng Anh
- ✅ Logic tính điểm chỉ cho lượt hiện tại
- ✅ Kiểm tra từ hợp lệ chứa chữ cái mới
- ✅ Giao diện đặt chữ cái
- ✅ Nút submit để validate và tính điểm
- ✅ Theo dõi chữ cái mới trong mỗi lượt

**Đây là cách chơi Scrabble chuẩn - chỉ tính điểm cho từ mới được tạo ra, không tính lại từ cũ!** 🎯
