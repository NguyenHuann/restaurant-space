# Dự Án Đồ Họa Máy Tính 3D (Không gian nhà hàng)

## Hướng Dẫn Điều Khiển

Tài liệu này cung cấp hướng dẫn chi tiết về các phím bấm và thao tác tương tác được lập trình trong hệ thống giả lập không gian nhà hàng 3D sử dụng **OpenGL** và **GLUT**.

---

## 1. Hệ Thống Điều Khiển Camera (Fly Camera)

Hệ thống tích hợp một camera tự do (Fly Camera) cho phép di chuyển linh hoạt trong không gian 3 chiều và thay đổi hướng nhìn theo 6 mức độ tự do (6 DoF).

### Di chuyển Vị trí (Position)

Sử dụng các phím sau để di chuyển tọa độ của camera trong phòng:

|   Phím bấm    | Thao tác tương ứng            | Mô tả chi tiết                                           |
| :-----------: | :---------------------------- | :------------------------------------------------------- |
| **W** / **w** | Di chuyển Tiến (`FORWARD`)    | Đưa camera tiến về phía trước theo hướng nhìn hiện tại.  |
| **S** / **s** | Di chuyển Lùi (`BACKWARD`)    | Đưa camera lùi lại phía sau.                             |
| **A** / **a** | Di chuyển Sang Trái (`LEFT`)  | Di chuyển camera tịnh tiến sang bên trái (Strafe Left).  |
| **D** / **d** | Di chuyển Sang Phải (`RIGHT`) | Di chuyển camera tịnh tiến sang bên phải (Strafe Right). |
| **E** / **e** | Di chuyển Lên Trên (`UP`)     | Tăng cao độ của camera (bay lên trên trần nhà).          |
| **R** / **r** | Di chuyển Xuống Dưới (`DOWN`) | Giảm cao độ của camera (hạ xuống sát mặt sàn).           |

### Thay đổi Góc nhìn (Rotation / Orientation)

Sử dụng các cặp phím sau để điều chỉnh các góc xoay của camera (Pitch, Yaw, Roll):

|   Phím bấm    | Trục xoay tương ứng      | Mô tả chi tiết                                               |
| :-----------: | :----------------------- | :----------------------------------------------------------- |
| **X** / **x** | Ngước lên (`P_UP`)       | Xoay góc Pitch lên trên (nhìn hướng lên trần nhà).           |
| **C** / **c** | Cúi xuống (`P_DOWN`)     | Xoay góc Pitch xuống dưới (nhìn hướng xuống mặt sàn).        |
| **Y** / **y** | Quay trái (`Y_LEFT`)     | Xoay góc Yaw sang trái (quay đầu nhìn sang bên trái).        |
| **V** / **v** | Quay phải (`Y_RIGHT`)    | Xoay góc Yaw sang phải (quay đầu nhìn sang bên phải).        |
| **Z** / **z** | Nghiêng trái (`R_LEFT`)  | Xoay góc Roll sang trái (nghiêng góc nhìn camera sang trái). |
| **Q** / **q** | Nghiêng phải (`R_RIGHT`) | Xoay góc Roll sang phải (nghiêng góc nhìn camera sang phải). |

---

## 2. Điều Khiển Tương Tác Môi Trường & Hệ Thống

Chương trình hỗ trợ các phím chức năng để kích hoạt các hiệu ứng động (Animation) của các vật thể trong phòng hoặc điều khiển trạng thái ứng dụng:

|   Phím bấm    | Chức năng tương ứng      | Mô tả chi tiết                                                                                           |
| :-----------: | :----------------------- | :------------------------------------------------------------------------------------------------------- |
| **G** / **g** | Bật / Tắt Quạt Trần      | Kích hoạt hiệu ứng quay của các cánh quạt trần (`fan_turn`). Mỗi khung hình góc xoay tăng thêm 10 độ.    |
| **F** / **f** | Bật / Tắt Camera Tự Động | Kích hoạt chế độ Cinematic quay camera tự động quanh phòng theo trục Yaw sang trái (`Y_LEFT`).           |
| **ESC** (27)  | Thoát Chương Trình       | Giải phóng vòng lặp sự kiện của FreeGLUT (`glutLeaveMainLoop`) và đóng cửa sổ ứng dụng một cách an toàn. |

---

## 3. Thao Tác Bằng Chuột (Mouse Interactions)

Bên cạnh bàn phím, chuột được sử dụng như một công cụ trực quan để thay đổi góc nhìn xung quanh (Look Around) tương tự như các tựa game góc nhìn thứ nhất (FPS):

- **Kích hoạt góc nhìn:** **Nhấn và GIỮ Chuột Phải (`GLUT_RIGHT_BUTTON`)**.
- **Xoay camera:** Khi đang giữ chuột phải, thực hiện **Di chuyển chuột** (`mouseMotionFunc`).
  - Di chuyển chuột sang Trái/Phải: Thay đổi góc nhìn theo phương ngang (Yaw).
  - Di chuyển chuột lên Lên/Xuống: Thay đổi góc nhìn theo phương dọc (Pitch).
- **Cơ chế chống giật:** Hệ thống tự động đặt lại tọa độ mốc (`firstMouse = true`) ngay khi bạn nhấn chuột phải để tránh tình trạng camera bị nhảy góc nhìn đột ngột.

---

## 4. Lưu ý Kỹ thuật dành cho Nhà phát triển

- Các thao tác di chuyển vị trí và góc nhìn đều được nhân với biến thời gian sai lệch giữa các khung hình `deltaTime` (tính từ `glutGet(GLUT_ELAPSED_TIME)`). Điều này đảm bảo tốc độ di chuyển của camera ổn định và mượt mà trên mọi cấu hình máy tính khác nhau, không bị phụ thuộc vào tốc độ khung hình (FPS).
- Cửa sổ hiển thị mặc định khi khởi chạy dự án có tiêu đề là `"Nha hang 3D"` với kích thước nền tảng `800x600` pixel.

---
