import cv2
import time
import numpy as np
from dataclasses import dataclass


# -----------------------------
# Processing arguments
# -----------------------------
@dataclass
class ProcessingArgs:
    grayScale: bool = False

    resize: bool = False
    resizeSize: tuple = (640, 480)
    resizeInterpolation: int = cv2.INTER_LINEAR

    gaussianBlur: bool = False
    gaussianBlurKernelSize: tuple = (5, 5)

    cannyEdge: bool = False
    cannyThreshold1: int = 200
    cannyThreshold2: int = 400

    dilate: bool = False
    dilate_size: int = 5

    findContours: bool = False
    findContoursRetrievalMode: int = cv2.RETR_EXTERNAL
    findContoursApproxMode: int = cv2.CHAIN_APPROX_SIMPLE


# -----------------------------
# Timing structures
# -----------------------------
@dataclass
class ProcessingTimings:
    grab_ms: float = 0.0
    image_to_mat_ms: float = 0.0
    processed_ms: float = 0.0
    back_to_rgba_ms: float = 0.0
    mat_to_image_ms: float = 0.0
    total_ms: float = 0.0


# -----------------------------
# Main tester
# -----------------------------
class OpenCVViewportTester:
    def __init__(self, processing_args: ProcessingArgs):
        self.processing_args = processing_args

        # per-second accumulator
        self.sec_acc = ProcessingTimings()
        self.sec_samples = 0

        # global accumulator
        self.global_acc = ProcessingTimings()
        self.global_samples = 0

    def _ms(self, a, b):
        return (b - a) * 1000.0

    def process_frame(self, frame_bgr: np.ndarray) -> np.ndarray:
        t_start = time.perf_counter()

        # --- Grab frame ---
        t0 = time.perf_counter()
        image = frame_bgr.copy()
        t1 = time.perf_counter()

        # --- Image -> Mat ---
        mat = image
        t2 = time.perf_counter()

        args = self.processing_args

        if args.grayScale:
            mat = cv2.cvtColor(mat, cv2.COLOR_BGR2GRAY)

        if args.resize:
            mat = cv2.resize(
                mat, args.resizeSize,
                interpolation=args.resizeInterpolation
            )

        if args.gaussianBlur:
            mat = cv2.GaussianBlur(mat, args.gaussianBlurKernelSize, 0)

        if args.dilate:
            k = args.dilate_size
            kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (k, k))
            mat = cv2.dilate(mat, kernel)

        edge_mat = None
        if args.cannyEdge:
            edge_mat = cv2.Canny(
                mat, args.cannyThreshold1, args.cannyThreshold2
            )

        if args.findContours:
            if args.cannyEdge:
                binary = edge_mat
            elif args.grayScale:
                _, binary = cv2.threshold(mat, 128, 255, cv2.THRESH_BINARY)
            else:
                gray = cv2.cvtColor(mat, cv2.COLOR_BGR2GRAY)
                _, binary = cv2.threshold(gray, 128, 255, cv2.THRESH_BINARY)

            contours, _ = cv2.findContours(
                binary,
                args.findContoursRetrievalMode,
                args.findContoursApproxMode
            )
            cv2.drawContours(mat, contours, -1, (255, 0, 0), 2)

        elif args.cannyEdge:
            mat = edge_mat

        t3 = time.perf_counter()

        # --- Back to BGRA ---
        if len(mat.shape) == 2:
            mat = cv2.cvtColor(mat, cv2.COLOR_GRAY2BGRA)
        elif mat.shape[2] == 3:
            mat = cv2.cvtColor(mat, cv2.COLOR_BGR2BGRA)

        t4 = time.perf_counter()

        # --- Mat -> Image ---
        result = mat
        t5 = time.perf_counter()

        # --- Timing ---
        grab = self._ms(t0, t1)
        i2m = self._ms(t1, t2)
        proc = self._ms(t2, t3)
        back = self._ms(t3, t4)
        m2i = self._ms(t4, t5)
        total = self._ms(t_start, t5)

        # per-second
        self.sec_acc.grab_ms += grab
        self.sec_acc.image_to_mat_ms += i2m
        self.sec_acc.processed_ms += proc
        self.sec_acc.back_to_rgba_ms += back
        self.sec_acc.mat_to_image_ms += m2i
        self.sec_acc.total_ms += total
        self.sec_samples += 1

        # global
        self.global_acc.grab_ms += grab
        self.global_acc.image_to_mat_ms += i2m
        self.global_acc.processed_ms += proc
        self.global_acc.back_to_rgba_ms += back
        self.global_acc.mat_to_image_ms += m2i
        self.global_acc.total_ms += total
        self.global_samples += 1

        return result

    def consume_second_average(self) -> ProcessingTimings:
        if self.sec_samples == 0:
            return ProcessingTimings()

        inv = 1.0 / self.sec_samples
        avg = ProcessingTimings(
            grab_ms=self.sec_acc.grab_ms * inv,
            image_to_mat_ms=self.sec_acc.image_to_mat_ms * inv,
            processed_ms=self.sec_acc.processed_ms * inv,
            back_to_rgba_ms=self.sec_acc.back_to_rgba_ms * inv,
            mat_to_image_ms=self.sec_acc.mat_to_image_ms * inv,
            total_ms=self.sec_acc.total_ms * inv,
        )

        self.sec_acc = ProcessingTimings()
        self.sec_samples = 0
        return avg

    def get_global_average(self) -> ProcessingTimings:
        inv = 1.0 / self.global_samples
        return ProcessingTimings(
            grab_ms=self.global_acc.grab_ms * inv,
            image_to_mat_ms=self.global_acc.image_to_mat_ms * inv,
            processed_ms=self.global_acc.processed_ms * inv,
            back_to_rgba_ms=self.global_acc.back_to_rgba_ms * inv,
            mat_to_image_ms=self.global_acc.mat_to_image_ms * inv,
            total_ms=self.global_acc.total_ms * inv,
        )

    @staticmethod
    def timings_to_string(t: ProcessingTimings, samples: int) -> str:
        return (
            f"{t.grab_ms:7.3f} ms  Grab from video\n"
            f"{t.image_to_mat_ms:7.3f} ms  Image to Mat\n"
            f"{t.processed_ms:7.3f} ms  Processing\n"
            f"{t.back_to_rgba_ms:7.3f} ms  Processed -> BGRA\n"
            f"{t.mat_to_image_ms:7.3f} ms  Mat to Image\n"
            f"= {t.total_ms:7.3f} ms  Total per Frame\n"
            f"Samples: {samples}\n"
        )


# -----------------------------
# Main (video.ogv)
# -----------------------------
if __name__ == "__main__":
    cap = cv2.VideoCapture("Brompton_MBOX_HDR-Selects_BT2020-ST2084_UHDp60_v2.ogv")
    if not cap.isOpened():
        raise RuntimeError("Failed to open video.ogv")

    args = ProcessingArgs(
        grayScale=True,
        resize=False,
        gaussianBlur=True,
        cannyEdge=True,
        findContours=True,
        dilate=True
    )

    tester = OpenCVViewportTester(args)
    last_print = time.perf_counter()

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        output = tester.process_frame(frame)
        cv2.imshow("Processed", output)

        now = time.perf_counter()
        if now - last_print >= 1.0:
            avg = tester.consume_second_average()
            print("=== Averaged Timings ===\n" + tester.timings_to_string(avg, tester.global_samples))
            last_print = now

        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

    # --- Final global stats ---
    final_avg = tester.get_global_average()
    stats = tester.timings_to_string(final_avg, tester.global_samples)

    with open("stats.txt", "w") as f:
        f.write(stats)

    print("Saved final timing statistics to stats.txt")
    print(stats)
