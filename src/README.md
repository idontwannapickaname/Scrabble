# Scrabble Game

Đây là một game Scrabble hoàn chỉnh với menu chính và game board.

## Tính năng

- **Menu chính**: Hiển thị tiêu đề "SCRABBLE" và các tùy chọn menu
- **Game board**: Bảng chơi 15x15 với các ô đặc biệt
- **Player tiles**: 7 tiles cho mỗi người chơi
- **Scoring system**: Hệ thống tính điểm theo giá trị tiles
- **Turn-based**: Chơi luân phiên giữa 2 người chơi
- **Text rendering**: Hiển thị text đẹp mắt với SDL2_ttf

## Cách sử dụng

### Điều khiển menu:
- **Phím mũi tên lên/xuống**: Di chuyển giữa các tùy chọn menu
- **Enter hoặc Space**: Chọn tùy chọn hiện tại
- **Escape**: Thoát game (tương đương với chọn Quit)

### Điều khiển game:
- **Mouse click**: Chọn tile hoặc vị trí trên board
- **Left/Right arrows**: Di chuyển giữa các tiles
- **Space**: Chọn tile để đặt
- **Enter**: Đặt tile đã chọn
- **Escape**: Thoát game

### Tùy chọn menu:
1. **Play**: Bắt đầu game Scrabble
2. **Quit**: Thoát khỏi game

## Yêu cầu hệ thống

- SDL2
- SDL2_ttf (cho hiển thị text)
- Font Arial, Calibri, Tahoma, Verdana, hoặc font hệ thống khác

## Cách biên dịch

1. Đảm bảo đã cài đặt SDL2 và SDL2_ttf
2. Mở project trong CodeBlocks
3. Build và chạy

## Cấu trúc code

- `main.cpp`: File chính chứa class SDL và hàm main
- `Menu.h`: Header file cho class Menu
- `Menu.cpp`: Implementation của class Menu
- `GameWindow.h`: Header file cho class GameWindow
- `GameWindow.cpp`: Implementation của class GameWindow
- `ScrabbleGame.h`: Header file cho class ScrabbleGame
- `ScrabbleGame.cpp`: Implementation của class ScrabbleGame

## Tính năng

- **Text rendering**: Hiển thị text đẹp mắt với SDL2_ttf
- **Font loading thông minh**: Tự động thử nhiều font khác nhau
- **Fallback rendering**: Hiển thị hình chữ nhật nếu không load được font
- **Selection indicator**: Hiển thị rõ ràng item được chọn
- **Error handling**: Xử lý lỗi tốt hơn khi không có font

## Lưu ý

- Khi chọn Play, sẽ mở cửa sổ game Scrabble với bảng chơi 15x15
- Game có hệ thống tính điểm theo giá trị tiles chuẩn
- Có thể thoát game bằng cách nhấn Escape hoặc đóng cửa sổ
- Menu sẽ hiển thị text đẹp mắt nếu có font phù hợp, ngược lại sẽ dùng hình chữ nhật 