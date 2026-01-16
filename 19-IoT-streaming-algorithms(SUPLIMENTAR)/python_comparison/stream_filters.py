#!/usr/bin/env python3
"""
============================================================================
STREAM FILTERS - Implementări Python pentru Comparație
============================================================================

Săptămâna 19: Algoritmi pentru IoT și Stream Processing
Curs: Algoritmi și Tehnici de Programare, ASE București

DESCRIERE:
    Implementări Python ale algoritmilor de filtrare stream processing
    pentru comparație cu versiunile C și demonstrare vizuală.

UTILIZARE:
    python3 stream_filters.py                    # Demo interactiv
    python3 stream_filters.py --file data.csv   # Procesare fișier
    python3 stream_filters.py --benchmark       # Benchmark performanță

DEPENDENȚE:
    pip install numpy matplotlib pandas

AUTOR: Generated for ASE-CSIE ATP Course
VERSIUNE: 1.0 (Ianuarie 2026)
============================================================================
"""

import numpy as np
import matplotlib.pyplot as plt
from dataclasses import dataclass
from typing import List, Tuple, Optional
import time
import argparse
import sys

# ==============================================================================
# IMPLEMENTĂRI FILTRE
# ==============================================================================

class MovingAverage:
    """
    Filtru Moving Average cu buffer circular.
    
    Complexitate: O(1) timp, O(k) spațiu
    """
    def __init__(self, window_size: int):
        self.window_size = window_size
        self.buffer = np.zeros(window_size)
        self.head = 0
        self.count = 0
        self.sum = 0.0
    
    def update(self, value: float) -> float:
        """Adaugă valoare și returnează media curentă."""
        # Scade valoarea veche din sumă
        if self.count == self.window_size:
            self.sum -= self.buffer[self.head]
        else:
            self.count += 1
        
        # Adaugă valoarea nouă
        self.buffer[self.head] = value
        self.sum += value
        
        # Avansează head-ul circular
        self.head = (self.head + 1) % self.window_size
        
        return self.sum / self.count
    
    def reset(self):
        """Resetează filtrul."""
        self.buffer.fill(0)
        self.head = 0
        self.count = 0
        self.sum = 0.0


class ExponentialMovingAverage:
    """
    Filtru Exponential Moving Average (EMA).
    
    Formula: EMA(t) = α × x(t) + (1-α) × EMA(t-1)
    
    Complexitate: O(1) timp, O(1) spațiu
    """
    def __init__(self, alpha: float = 0.2):
        if not 0 < alpha <= 1:
            raise ValueError("Alpha trebuie să fie în (0, 1]")
        self.alpha = alpha
        self.value = 0.0
        self.initialized = False
    
    def update(self, measurement: float) -> float:
        """Actualizează EMA cu noua măsurătoare."""
        if not self.initialized:
            self.value = measurement
            self.initialized = True
        else:
            self.value = self.alpha * measurement + (1 - self.alpha) * self.value
        return self.value
    
    @property
    def equivalent_window(self) -> float:
        """Returnează dimensiunea echivalentă a ferestrei MA."""
        return (2 / self.alpha) - 1
    
    def reset(self):
        """Resetează filtrul."""
        self.value = 0.0
        self.initialized = False


class KalmanFilter1D:
    """
    Filtru Kalman simplificat pentru 1 dimensiune.
    
    Ciclul: PREDICT → UPDATE
    
    Complexitate: O(1) timp, O(1) spațiu
    """
    def __init__(self, process_noise: float = 0.01, measurement_noise: float = 0.1):
        self.Q = process_noise      # Zgomot proces
        self.R = measurement_noise  # Zgomot măsurătoare
        self.x = 0.0               # Estimare stare
        self.P = 1.0               # Varianța estimării
        self.initialized = False
    
    def update(self, measurement: float) -> float:
        """Aplică ciclul predict-update și returnează estimarea."""
        if not self.initialized:
            self.x = measurement
            self.initialized = True
            return self.x
        
        # === PREDICT ===
        # x_pred = x (model static - starea nu se schimbă singură)
        x_pred = self.x
        # P_pred = P + Q
        P_pred = self.P + self.Q
        
        # === UPDATE ===
        # Kalman Gain: K = P_pred / (P_pred + R)
        K = P_pred / (P_pred + self.R)
        
        # Actualizare estimare: x = x_pred + K * (z - x_pred)
        self.x = x_pred + K * (measurement - x_pred)
        
        # Actualizare varianță: P = (1 - K) * P_pred
        self.P = (1 - K) * P_pred
        
        return self.x
    
    @property
    def kalman_gain(self) -> float:
        """Returnează câștigul Kalman curent."""
        P_pred = self.P + self.Q
        return P_pred / (P_pred + self.R)
    
    def reset(self):
        """Resetează filtrul."""
        self.x = 0.0
        self.P = 1.0
        self.initialized = False


@dataclass
class FilterComparison:
    """Rezultatele comparației între filtre."""
    raw: np.ndarray
    ma: np.ndarray
    ema: np.ndarray
    kalman: np.ndarray
    timestamps: np.ndarray


# ==============================================================================
# FUNCȚII DE GENERARE DATE
# ==============================================================================

def generate_sensor_data(
    n_samples: int = 1000,
    base_value: float = 25.0,
    noise_std: float = 1.0,
    anomaly_prob: float = 0.02,
    anomaly_magnitude: float = 10.0,
    seed: Optional[int] = None
) -> Tuple[np.ndarray, np.ndarray]:
    """
    Generează date simulate de senzor cu zgomot și anomalii.
    
    Args:
        n_samples: Numărul de eșantioane
        base_value: Valoarea de bază (ex: temperatura medie)
        noise_std: Deviația standard a zgomotului Gaussian
        anomaly_prob: Probabilitatea unei anomalii
        anomaly_magnitude: Magnitudinea maximă a anomaliilor
        seed: Seed pentru reproducibilitate
    
    Returns:
        timestamps: Array cu timpii [0, 1, 2, ...]
        values: Array cu valorile generate
    """
    if seed is not None:
        np.random.seed(seed)
    
    # Timestamps
    timestamps = np.arange(n_samples)
    
    # Valoare de bază cu variație sinusoidală (simulare ciclu zi/noapte)
    daily_variation = 3.0 * np.sin(2 * np.pi * timestamps / 100)
    
    # Zgomot Gaussian
    noise = np.random.normal(0, noise_std, n_samples)
    
    # Anomalii aleatorii
    anomalies = np.zeros(n_samples)
    anomaly_mask = np.random.random(n_samples) < anomaly_prob
    anomalies[anomaly_mask] = np.random.uniform(
        -anomaly_magnitude, anomaly_magnitude, 
        np.sum(anomaly_mask)
    )
    
    # Valori finale
    values = base_value + daily_variation + noise + anomalies
    
    return timestamps, values


def process_with_filters(
    values: np.ndarray,
    ma_window: int = 10,
    ema_alpha: float = 0.2,
    kalman_q: float = 0.01,
    kalman_r: float = 0.5
) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    Procesează valorile prin toate cele 3 filtre.
    
    Returns:
        ma_output, ema_output, kalman_output
    """
    ma_filter = MovingAverage(ma_window)
    ema_filter = ExponentialMovingAverage(ema_alpha)
    kalman_filter = KalmanFilter1D(kalman_q, kalman_r)
    
    ma_output = np.zeros(len(values))
    ema_output = np.zeros(len(values))
    kalman_output = np.zeros(len(values))
    
    for i, value in enumerate(values):
        ma_output[i] = ma_filter.update(value)
        ema_output[i] = ema_filter.update(value)
        kalman_output[i] = kalman_filter.update(value)
    
    return ma_output, ema_output, kalman_output


# ==============================================================================
# VIZUALIZARE
# ==============================================================================

def plot_filter_comparison(
    comparison: FilterComparison,
    title: str = "Comparație Filtre Stream Processing",
    save_path: Optional[str] = None
):
    """
    Generează grafic comparativ pentru filtre.
    """
    fig, axes = plt.subplots(2, 1, figsize=(14, 10), sharex=True)
    
    # Grafic 1: Toate semnalele
    ax1 = axes[0]
    ax1.plot(comparison.timestamps, comparison.raw, 
             'lightgray', linewidth=0.8, label='Date brute', alpha=0.7)
    ax1.plot(comparison.timestamps, comparison.ma, 
             'blue', linewidth=1.5, label=f'Moving Average')
    ax1.plot(comparison.timestamps, comparison.ema, 
             'green', linewidth=1.5, label=f'EMA')
    ax1.plot(comparison.timestamps, comparison.kalman, 
             'red', linewidth=1.5, label=f'Kalman Filter')
    
    ax1.set_ylabel('Valoare', fontsize=12)
    ax1.set_title(title, fontsize=14, fontweight='bold')
    ax1.legend(loc='upper right')
    ax1.grid(True, alpha=0.3)
    
    # Grafic 2: Eroarea față de trend real (aproximat prin media mobilă mare)
    trend = np.convolve(comparison.raw, np.ones(50)/50, mode='same')
    
    ax2 = axes[1]
    ax2.plot(comparison.timestamps, np.abs(comparison.ma - trend), 
             'blue', linewidth=1, label='Eroare MA', alpha=0.7)
    ax2.plot(comparison.timestamps, np.abs(comparison.ema - trend), 
             'green', linewidth=1, label='Eroare EMA', alpha=0.7)
    ax2.plot(comparison.timestamps, np.abs(comparison.kalman - trend), 
             'red', linewidth=1, label='Eroare Kalman', alpha=0.7)
    
    ax2.set_xlabel('Timestamp', fontsize=12)
    ax2.set_ylabel('Eroare Absolută', fontsize=12)
    ax2.set_title('Eroarea Filtrelor față de Trend', fontsize=12)
    ax2.legend(loc='upper right')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"Grafic salvat în: {save_path}")
    else:
        plt.show()


def plot_step_response(save_path: Optional[str] = None):
    """
    Vizualizează răspunsul filtrelor la o schimbare bruscă (step).
    """
    # Generare step signal
    n = 100
    signal = np.concatenate([np.zeros(50), np.ones(50) * 10])
    
    # Procesare
    ma10 = MovingAverage(10)
    ma20 = MovingAverage(20)
    ema_fast = ExponentialMovingAverage(0.3)
    ema_slow = ExponentialMovingAverage(0.1)
    kalman = KalmanFilter1D(0.01, 0.5)
    
    results = {
        'MA-10': [],
        'MA-20': [],
        'EMA α=0.3': [],
        'EMA α=0.1': [],
        'Kalman': []
    }
    
    for val in signal:
        results['MA-10'].append(ma10.update(val))
        results['MA-20'].append(ma20.update(val))
        results['EMA α=0.3'].append(ema_fast.update(val))
        results['EMA α=0.1'].append(ema_slow.update(val))
        results['Kalman'].append(kalman.update(val))
    
    # Plot
    fig, ax = plt.subplots(figsize=(12, 6))
    
    ax.step(range(n), signal, 'k--', linewidth=2, label='Step Input', where='post')
    
    colors = ['blue', 'lightblue', 'green', 'lightgreen', 'red']
    for (name, data), color in zip(results.items(), colors):
        ax.plot(data, color=color, linewidth=1.5, label=name)
    
    ax.axvline(x=50, color='gray', linestyle=':', alpha=0.5)
    ax.set_xlabel('Sample', fontsize=12)
    ax.set_ylabel('Valoare', fontsize=12)
    ax.set_title('Răspunsul Filtrelor la Step Input', fontsize=14, fontweight='bold')
    ax.legend(loc='lower right')
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
    else:
        plt.show()


# ==============================================================================
# BENCHMARK
# ==============================================================================

def benchmark_filters(n_samples: int = 100000, n_runs: int = 10):
    """
    Benchmark performanță pentru filtre.
    """
    print(f"\n{'='*60}")
    print(f"BENCHMARK: {n_samples:,} samples, {n_runs} runs")
    print(f"{'='*60}\n")
    
    # Generare date
    _, values = generate_sensor_data(n_samples, seed=42)
    
    filters = {
        'Moving Average (k=10)': lambda: MovingAverage(10),
        'Moving Average (k=50)': lambda: MovingAverage(50),
        'EMA (α=0.2)': lambda: ExponentialMovingAverage(0.2),
        'EMA (α=0.1)': lambda: ExponentialMovingAverage(0.1),
        'Kalman 1D': lambda: KalmanFilter1D(0.01, 0.5),
    }
    
    results = {}
    
    for name, create_filter in filters.items():
        times = []
        
        for _ in range(n_runs):
            f = create_filter()
            
            start = time.perf_counter()
            for val in values:
                f.update(val)
            elapsed = time.perf_counter() - start
            
            times.append(elapsed)
        
        avg_time = np.mean(times) * 1000  # ms
        std_time = np.std(times) * 1000
        throughput = n_samples / np.mean(times)
        
        results[name] = {
            'avg_ms': avg_time,
            'std_ms': std_time,
            'throughput': throughput
        }
        
        print(f"{name:30} {avg_time:8.2f} ± {std_time:.2f} ms  "
              f"({throughput:,.0f} samples/sec)")
    
    print(f"\n{'='*60}\n")
    
    return results


# ==============================================================================
# CLI
# ==============================================================================

def main():
    parser = argparse.ArgumentParser(
        description='Stream Filters - Demo și Benchmark',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Exemple:
  python stream_filters.py                    # Demo interactiv
  python stream_filters.py --benchmark        # Benchmark performanță
  python stream_filters.py --save plot.png    # Salvează grafic
        """
    )
    
    parser.add_argument('--benchmark', action='store_true',
                       help='Rulează benchmark performanță')
    parser.add_argument('--samples', type=int, default=1000,
                       help='Număr de eșantioane pentru demo (default: 1000)')
    parser.add_argument('--save', type=str, default=None,
                       help='Salvează graficul în fișier')
    parser.add_argument('--step-response', action='store_true',
                       help='Afișează răspunsul la step')
    parser.add_argument('--seed', type=int, default=42,
                       help='Seed pentru reproducibilitate')
    
    args = parser.parse_args()
    
    print("\n" + "="*60)
    print("  STREAM FILTERS - ATP Week 19")
    print("  Algoritmi pentru IoT și Stream Processing")
    print("="*60 + "\n")
    
    if args.benchmark:
        benchmark_filters()
        return
    
    if args.step_response:
        plot_step_response(args.save)
        return
    
    # Demo standard
    print(f"Generare {args.samples} eșantioane simulate...")
    timestamps, values = generate_sensor_data(args.samples, seed=args.seed)
    
    print("Procesare prin filtre...")
    ma_out, ema_out, kalman_out = process_with_filters(values)
    
    # Statistici
    print("\nStatistici:")
    print(f"  Date brute:   μ={np.mean(values):.2f}, σ={np.std(values):.2f}")
    print(f"  Moving Avg:   μ={np.mean(ma_out):.2f}, σ={np.std(ma_out):.2f}")
    print(f"  EMA:          μ={np.mean(ema_out):.2f}, σ={np.std(ema_out):.2f}")
    print(f"  Kalman:       μ={np.mean(kalman_out):.2f}, σ={np.std(kalman_out):.2f}")
    
    # Creare obiect comparație
    comparison = FilterComparison(
        raw=values,
        ma=ma_out,
        ema=ema_out,
        kalman=kalman_out,
        timestamps=timestamps
    )
    
    # Vizualizare
    print("\nGenerare grafic...")
    plot_filter_comparison(comparison, save_path=args.save)
    
    print("\nGata!")


if __name__ == '__main__':
    main()
