#!/usr/bin/env python3
"""
============================================================================
ANOMALY DETECTION - Implementări Python pentru Comparație
============================================================================

Săptămâna 19: Algoritmi pentru IoT și Stream Processing
Curs: Algoritmi și Tehnici de Programare, ASE București

DESCRIERE:
    Implementări Python ale algoritmilor de detecție anomalii pentru
    stream processing, inclusiv Z-score cu Welford și IQR online.

UTILIZARE:
    python3 anomaly_detection.py                    # Demo cu date simulate
    python3 anomaly_detection.py --file data.csv   # Analiză fișier
    python3 anomaly_detection.py --realtime        # Simulare timp real

DEPENDENȚE:
    pip install numpy matplotlib pandas

AUTOR: Generated for ASE-CSIE ATP Course
VERSIUNE: 1.0 (Ianuarie 2026)
============================================================================
"""

import numpy as np
import matplotlib.pyplot as plt
from dataclasses import dataclass, field
from typing import List, Tuple, Optional
from collections import deque
import time
import argparse


# ==============================================================================
# ALGORITMI DETECȚIE ANOMALII
# ==============================================================================

class ZScoreDetector:
    """
    Detector de anomalii bazat pe Z-Score.
    
    Folosește algoritmul Welford pentru calcul incremental al mediei
    și varianței, permițând detecție în O(1) per valoare.
    
    Formula: Z = (x - μ) / σ
    Anomalie dacă |Z| > threshold (tipic 2.5-3.0)
    """
    
    def __init__(self, threshold: float = 2.5, warmup_samples: int = 30):
        """
        Args:
            threshold: Pragul Z-score peste care o valoare e anomalie
            warmup_samples: Număr minim de eșantioane înainte de detecție
        """
        self.threshold = threshold
        self.warmup_samples = warmup_samples
        
        # Statistici Welford
        self.mean = 0.0
        self.M2 = 0.0      # Suma pătratelor diferențelor
        self.count = 0
        
        # Statistici detecție
        self.anomalies_detected = 0
        self.last_zscore = 0.0
    
    def update(self, value: float) -> Tuple[bool, float]:
        """
        Verifică dacă valoarea e anomalie și actualizează statisticile.
        
        Returns:
            (is_anomaly, zscore)
        """
        self.count += 1
        
        # Algoritmul Welford - actualizare incrementală stabilă numeric
        delta = value - self.mean
        self.mean += delta / self.count
        delta2 = value - self.mean
        self.M2 += delta * delta2
        
        # Perioada de warmup - nu detectăm anomalii
        if self.count < self.warmup_samples:
            return False, 0.0
        
        # Calculăm deviația standard
        variance = self.M2 / self.count
        stddev = np.sqrt(variance)
        
        # Evităm împărțirea la zero
        if stddev < 1e-10:
            return False, 0.0
        
        # Z-score
        zscore = abs(delta) / stddev
        self.last_zscore = zscore
        
        # Verificare anomalie
        is_anomaly = zscore > self.threshold
        if is_anomaly:
            self.anomalies_detected += 1
        
        return is_anomaly, zscore
    
    @property
    def stddev(self) -> float:
        """Deviația standard curentă."""
        return np.sqrt(self.M2 / self.count) if self.count > 1 else 0.0
    
    def reset(self):
        """Resetează detectorul."""
        self.mean = 0.0
        self.M2 = 0.0
        self.count = 0
        self.anomalies_detected = 0


class IQRDetector:
    """
    Detector de anomalii bazat pe Interquartile Range (IQR).
    
    Folosește un buffer sliding pentru a menține percentilele.
    Mai robust la outliers decât Z-score, dar necesită mai multă memorie.
    
    Anomalie dacă: x < Q1 - k*IQR sau x > Q3 + k*IQR (tipic k=1.5)
    """
    
    def __init__(self, window_size: int = 100, k: float = 1.5):
        """
        Args:
            window_size: Dimensiunea ferestrei sliding
            k: Multiplicator IQR pentru praguri (1.5 standard, 3.0 extreme)
        """
        self.window_size = window_size
        self.k = k
        
        self.buffer = deque(maxlen=window_size)
        self.anomalies_detected = 0
        
        # Cache pentru percentile (recalculare periodică)
        self._q1 = None
        self._q3 = None
        self._update_interval = max(10, window_size // 10)
        self._updates_since_recalc = 0
    
    def _recalculate_quartiles(self):
        """Recalculează Q1 și Q3 din buffer."""
        if len(self.buffer) < 4:
            return
        
        sorted_data = sorted(self.buffer)
        n = len(sorted_data)
        self._q1 = sorted_data[n // 4]
        self._q3 = sorted_data[3 * n // 4]
    
    def update(self, value: float) -> Tuple[bool, dict]:
        """
        Verifică dacă valoarea e anomalie.
        
        Returns:
            (is_anomaly, details_dict)
        """
        self.buffer.append(value)
        self._updates_since_recalc += 1
        
        # Recalculare periodică a percentilelor
        if self._updates_since_recalc >= self._update_interval:
            self._recalculate_quartiles()
            self._updates_since_recalc = 0
        
        # Warmup - buffer prea mic
        if len(self.buffer) < self.window_size // 2:
            return False, {}
        
        if self._q1 is None:
            self._recalculate_quartiles()
        
        # Calculare IQR și praguri
        iqr = self._q3 - self._q1
        lower_bound = self._q1 - self.k * iqr
        upper_bound = self._q3 + self.k * iqr
        
        # Verificare anomalie
        is_anomaly = value < lower_bound or value > upper_bound
        if is_anomaly:
            self.anomalies_detected += 1
        
        details = {
            'q1': self._q1,
            'q3': self._q3,
            'iqr': iqr,
            'lower_bound': lower_bound,
            'upper_bound': upper_bound
        }
        
        return is_anomaly, details
    
    def reset(self):
        """Resetează detectorul."""
        self.buffer.clear()
        self._q1 = None
        self._q3 = None
        self.anomalies_detected = 0


class AdaptiveZScoreDetector:
    """
    Detector Z-Score adaptiv cu fereastră sliding.
    
    Combină avantajele Z-score (O(1)) cu adaptabilitatea la
    schimbări în distribuția datelor (drift).
    """
    
    def __init__(self, window_size: int = 100, threshold: float = 2.5):
        self.window_size = window_size
        self.threshold = threshold
        
        self.buffer = deque(maxlen=window_size)
        self.sum = 0.0
        self.sum_sq = 0.0
        self.anomalies_detected = 0
    
    def update(self, value: float) -> Tuple[bool, float]:
        """
        Verifică anomalie folosind statistici din fereastră sliding.
        """
        # Elimină valoarea veche din sume
        if len(self.buffer) == self.window_size:
            old_value = self.buffer[0]
            self.sum -= old_value
            self.sum_sq -= old_value ** 2
        
        # Adaugă valoarea nouă
        self.buffer.append(value)
        self.sum += value
        self.sum_sq += value ** 2
        
        n = len(self.buffer)
        
        # Warmup
        if n < self.window_size // 2:
            return False, 0.0
        
        # Calculează statistici din fereastră
        mean = self.sum / n
        variance = (self.sum_sq / n) - (mean ** 2)
        stddev = np.sqrt(max(0, variance))
        
        if stddev < 1e-10:
            return False, 0.0
        
        zscore = abs(value - mean) / stddev
        
        is_anomaly = zscore > self.threshold
        if is_anomaly:
            self.anomalies_detected += 1
        
        return is_anomaly, zscore
    
    def reset(self):
        self.buffer.clear()
        self.sum = 0.0
        self.sum_sq = 0.0
        self.anomalies_detected = 0


class RateLimiter:
    """
    Rate limiter pentru prevenirea alert fatigue.
    
    Permite maximum N alerte într-o fereastră de timp specificată.
    """
    
    def __init__(self, max_alerts: int = 5, window_seconds: float = 60.0):
        self.max_alerts = max_alerts
        self.window_seconds = window_seconds
        
        self.timestamps = deque(maxlen=max_alerts)
        self.suppressed_count = 0
    
    def allow(self) -> bool:
        """Verifică dacă o alertă poate fi trimisă."""
        now = time.time()
        
        # Elimină timestamps expirate
        while self.timestamps and now - self.timestamps[0] > self.window_seconds:
            self.timestamps.popleft()
        
        # Verifică dacă mai avem loc
        if len(self.timestamps) >= self.max_alerts:
            self.suppressed_count += 1
            return False
        
        self.timestamps.append(now)
        return True


# ==============================================================================
# FUNCȚII DE GENERARE DATE ȘI ANALIZĂ
# ==============================================================================

def generate_data_with_anomalies(
    n_samples: int = 1000,
    base_value: float = 25.0,
    noise_std: float = 1.0,
    drift_rate: float = 0.001,
    anomaly_prob: float = 0.02,
    anomaly_magnitude: float = 10.0,
    seed: Optional[int] = None
) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    Generează date simulate cu anomalii marcate.
    
    Returns:
        values: valorile generate
        is_anomaly_ground_truth: array boolean cu anomaliile reale
        anomaly_indices: indicii anomaliilor
    """
    if seed is not None:
        np.random.seed(seed)
    
    # Valoare de bază cu drift lent
    drift = np.cumsum(np.random.normal(0, drift_rate, n_samples))
    
    # Variație sinusoidală (simulare ciclu)
    daily_variation = 3.0 * np.sin(2 * np.pi * np.arange(n_samples) / 100)
    
    # Zgomot Gaussian
    noise = np.random.normal(0, noise_std, n_samples)
    
    # Generare anomalii
    anomaly_mask = np.random.random(n_samples) < anomaly_prob
    anomalies = np.zeros(n_samples)
    anomalies[anomaly_mask] = np.random.choice([-1, 1], np.sum(anomaly_mask)) * \
                              np.random.uniform(anomaly_magnitude * 0.5, 
                                                anomaly_magnitude * 1.5,
                                                np.sum(anomaly_mask))
    
    values = base_value + drift + daily_variation + noise + anomalies
    
    return values, anomaly_mask, np.where(anomaly_mask)[0]


def evaluate_detector(
    y_true: np.ndarray,
    y_pred: np.ndarray
) -> dict:
    """
    Evaluează performanța unui detector.
    
    Returns:
        dict cu metrici: precision, recall, f1, accuracy
    """
    tp = np.sum(y_true & y_pred)
    fp = np.sum(~y_true & y_pred)
    fn = np.sum(y_true & ~y_pred)
    tn = np.sum(~y_true & ~y_pred)
    
    precision = tp / (tp + fp) if (tp + fp) > 0 else 0
    recall = tp / (tp + fn) if (tp + fn) > 0 else 0
    f1 = 2 * precision * recall / (precision + recall) if (precision + recall) > 0 else 0
    accuracy = (tp + tn) / len(y_true)
    
    return {
        'precision': precision,
        'recall': recall,
        'f1': f1,
        'accuracy': accuracy,
        'true_positives': int(tp),
        'false_positives': int(fp),
        'false_negatives': int(fn),
        'true_negatives': int(tn)
    }


def compare_detectors(
    values: np.ndarray,
    ground_truth: np.ndarray
) -> dict:
    """
    Compară toți detectoarele pe același set de date.
    """
    detectors = {
        'Z-Score (θ=2.5)': ZScoreDetector(threshold=2.5),
        'Z-Score (θ=3.0)': ZScoreDetector(threshold=3.0),
        'IQR (k=1.5)': IQRDetector(k=1.5),
        'IQR (k=3.0)': IQRDetector(k=3.0),
        'Adaptive Z (w=100)': AdaptiveZScoreDetector(window_size=100),
        'Adaptive Z (w=50)': AdaptiveZScoreDetector(window_size=50),
    }
    
    results = {}
    
    for name, detector in detectors.items():
        predictions = np.zeros(len(values), dtype=bool)
        
        for i, value in enumerate(values):
            if isinstance(detector, IQRDetector):
                is_anomaly, _ = detector.update(value)
            else:
                is_anomaly, _ = detector.update(value)
            predictions[i] = is_anomaly
        
        metrics = evaluate_detector(ground_truth, predictions)
        metrics['detector'] = name
        results[name] = metrics
    
    return results


# ==============================================================================
# VIZUALIZARE
# ==============================================================================

def plot_anomaly_detection(
    values: np.ndarray,
    ground_truth: np.ndarray,
    detected: np.ndarray,
    title: str = "Detecție Anomalii",
    save_path: Optional[str] = None
):
    """
    Vizualizează rezultatele detecției de anomalii.
    """
    fig, axes = plt.subplots(3, 1, figsize=(14, 10), sharex=True)
    
    # Grafic 1: Date și anomalii detectate
    ax1 = axes[0]
    ax1.plot(values, 'b-', linewidth=0.8, alpha=0.7, label='Date')
    
    # True positives (verde)
    tp_mask = ground_truth & detected
    ax1.scatter(np.where(tp_mask)[0], values[tp_mask], 
                c='green', s=50, marker='o', label='True Positive', zorder=5)
    
    # False positives (roșu)
    fp_mask = ~ground_truth & detected
    ax1.scatter(np.where(fp_mask)[0], values[fp_mask],
                c='red', s=50, marker='x', label='False Positive', zorder=5)
    
    # False negatives (portocaliu)
    fn_mask = ground_truth & ~detected
    ax1.scatter(np.where(fn_mask)[0], values[fn_mask],
                c='orange', s=50, marker='^', label='False Negative', zorder=5)
    
    ax1.set_ylabel('Valoare')
    ax1.set_title(title, fontsize=14, fontweight='bold')
    ax1.legend(loc='upper right')
    ax1.grid(True, alpha=0.3)
    
    # Grafic 2: Ground truth vs predicted
    ax2 = axes[1]
    ax2.fill_between(range(len(values)), 0, ground_truth.astype(int),
                     alpha=0.3, color='green', label='Ground Truth')
    ax2.fill_between(range(len(values)), 0, detected.astype(int),
                     alpha=0.3, color='blue', label='Detectate')
    ax2.set_ylabel('Anomalie')
    ax2.set_ylim(-0.1, 1.5)
    ax2.legend(loc='upper right')
    ax2.grid(True, alpha=0.3)
    
    # Grafic 3: Rolling accuracy
    window = 50
    rolling_correct = np.convolve(
        (ground_truth == detected).astype(float),
        np.ones(window) / window,
        mode='valid'
    )
    ax3 = axes[2]
    ax3.plot(range(window-1, len(values)), rolling_correct, 'purple', linewidth=1)
    ax3.axhline(y=0.9, color='green', linestyle='--', alpha=0.5, label='90% target')
    ax3.set_xlabel('Sample')
    ax3.set_ylabel('Rolling Accuracy')
    ax3.set_ylim(0, 1.05)
    ax3.legend(loc='lower right')
    ax3.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"Grafic salvat în: {save_path}")
    else:
        plt.show()


def plot_detector_comparison(results: dict, save_path: Optional[str] = None):
    """
    Grafic comparativ pentru detectoare.
    """
    detectors = list(results.keys())
    metrics = ['precision', 'recall', 'f1', 'accuracy']
    
    x = np.arange(len(detectors))
    width = 0.2
    
    fig, ax = plt.subplots(figsize=(14, 6))
    
    colors = ['#2ecc71', '#3498db', '#9b59b6', '#e74c3c']
    
    for i, metric in enumerate(metrics):
        values = [results[d][metric] for d in detectors]
        offset = (i - len(metrics)/2 + 0.5) * width
        bars = ax.bar(x + offset, values, width, label=metric.capitalize(), color=colors[i])
        
        # Adaugă valori pe bare
        for bar, val in zip(bars, values):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.02,
                   f'{val:.2f}', ha='center', va='bottom', fontsize=8)
    
    ax.set_ylabel('Score')
    ax.set_title('Comparație Performanță Detectoare Anomalii', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(detectors, rotation=15, ha='right')
    ax.legend(loc='upper right')
    ax.set_ylim(0, 1.15)
    ax.grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
    else:
        plt.show()


# ==============================================================================
# DEMO REAL-TIME
# ==============================================================================

def realtime_demo(duration_seconds: float = 30.0):
    """
    Demo în timp real cu actualizare grafică.
    """
    import matplotlib
    matplotlib.use('TkAgg')  # Backend pentru actualizare live
    
    plt.ion()
    fig, ax = plt.subplots(figsize=(12, 6))
    
    detector = ZScoreDetector(threshold=2.5)
    values = []
    detected = []
    zscores = []
    
    line_data, = ax.plot([], [], 'b-', linewidth=1, label='Date')
    scatter_anomaly = ax.scatter([], [], c='red', s=100, marker='x', label='Anomalie')
    
    ax.set_xlim(0, 100)
    ax.set_ylim(20, 30)
    ax.set_xlabel('Sample')
    ax.set_ylabel('Valoare')
    ax.set_title('Detecție Anomalii în Timp Real')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    start_time = time.time()
    sample_idx = 0
    
    print("Demo real-time pornit. Ctrl+C pentru oprire.")
    
    try:
        while time.time() - start_time < duration_seconds:
            # Generare valoare
            base = 25.0 + 2.0 * np.sin(sample_idx / 10)
            noise = np.random.normal(0, 0.5)
            
            # Anomalie ocazională
            if np.random.random() < 0.05:
                anomaly = np.random.choice([-1, 1]) * np.random.uniform(5, 10)
            else:
                anomaly = 0
            
            value = base + noise + anomaly
            
            # Detecție
            is_anomaly, zscore = detector.update(value)
            
            values.append(value)
            detected.append(is_anomaly)
            zscores.append(zscore)
            
            # Actualizare grafic
            n = len(values)
            line_data.set_data(range(n), values)
            
            anomaly_idx = [i for i, d in enumerate(detected) if d]
            anomaly_vals = [values[i] for i in anomaly_idx]
            scatter_anomaly.set_offsets(np.c_[anomaly_idx, anomaly_vals] if anomaly_idx else np.empty((0, 2)))
            
            if n > 100:
                ax.set_xlim(n - 100, n + 10)
            
            ax.set_ylim(min(values[-100:]) - 2, max(values[-100:]) + 2)
            
            fig.canvas.draw()
            fig.canvas.flush_events()
            
            sample_idx += 1
            time.sleep(0.1)
    
    except KeyboardInterrupt:
        print("\nDemo oprit.")
    
    plt.ioff()
    plt.show()
    
    print(f"\nRezultate:")
    print(f"  Total samples: {len(values)}")
    print(f"  Anomalii detectate: {sum(detected)}")
    print(f"  Rata anomalii: {100*sum(detected)/len(values):.1f}%")


# ==============================================================================
# CLI
# ==============================================================================

def main():
    parser = argparse.ArgumentParser(
        description='Anomaly Detection - Demo și Comparație',
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument('--realtime', action='store_true',
                       help='Rulează demo în timp real')
    parser.add_argument('--compare', action='store_true',
                       help='Compară toți detectoarele')
    parser.add_argument('--samples', type=int, default=1000,
                       help='Număr de eșantioane (default: 1000)')
    parser.add_argument('--anomaly-rate', type=float, default=0.03,
                       help='Rata anomaliilor (default: 0.03)')
    parser.add_argument('--save', type=str, default=None,
                       help='Salvează graficul în fișier')
    parser.add_argument('--seed', type=int, default=42,
                       help='Seed pentru reproducibilitate')
    
    args = parser.parse_args()
    
    print("\n" + "="*60)
    print("  ANOMALY DETECTION - ATP Week 19")
    print("  Algoritmi pentru IoT și Stream Processing")
    print("="*60 + "\n")
    
    if args.realtime:
        realtime_demo()
        return
    
    # Generare date
    print(f"Generare {args.samples} eșantioane cu {args.anomaly_rate*100:.1f}% anomalii...")
    values, ground_truth, anomaly_indices = generate_data_with_anomalies(
        n_samples=args.samples,
        anomaly_prob=args.anomaly_rate,
        seed=args.seed
    )
    
    print(f"Anomalii reale: {len(anomaly_indices)}")
    
    if args.compare:
        print("\nComparare detectoare...")
        results = compare_detectors(values, ground_truth)
        
        print("\n" + "-"*60)
        print(f"{'Detector':<25} {'Precision':>10} {'Recall':>10} {'F1':>10}")
        print("-"*60)
        for name, metrics in results.items():
            print(f"{name:<25} {metrics['precision']:>10.3f} {metrics['recall']:>10.3f} {metrics['f1']:>10.3f}")
        print("-"*60)
        
        plot_detector_comparison(results, args.save)
        return
    
    # Demo simplu cu un detector
    print("\nRulare detector Z-Score...")
    detector = ZScoreDetector(threshold=2.5)
    
    detected = np.zeros(len(values), dtype=bool)
    zscores = np.zeros(len(values))
    
    for i, value in enumerate(values):
        is_anomaly, zscore = detector.update(value)
        detected[i] = is_anomaly
        zscores[i] = zscore
    
    # Evaluare
    metrics = evaluate_detector(ground_truth, detected)
    
    print(f"\nRezultate Z-Score Detector:")
    print(f"  Precision: {metrics['precision']:.3f}")
    print(f"  Recall:    {metrics['recall']:.3f}")
    print(f"  F1 Score:  {metrics['f1']:.3f}")
    print(f"  Accuracy:  {metrics['accuracy']:.3f}")
    print(f"\n  True Positives:  {metrics['true_positives']}")
    print(f"  False Positives: {metrics['false_positives']}")
    print(f"  False Negatives: {metrics['false_negatives']}")
    
    # Vizualizare
    plot_anomaly_detection(values, ground_truth, detected, 
                          title="Detecție Anomalii cu Z-Score (θ=2.5)",
                          save_path=args.save)


if __name__ == '__main__':
    main()
