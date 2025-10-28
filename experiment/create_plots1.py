import matplotlib
matplotlib.use('Agg')  # Απενεργοποιεί το GUI backend για να μην εμφανίζονται warnings

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

def create_comparison_plots(df):
    """Δημιουργεί γραφήματα σύγκρισης"""

    # 1. BAR CHART: Recall@N ανά αλγόριθμο
    plt.figure(figsize=(10, 6))
    recall_by_algorithm = df.groupby('Algorithm')['Recall@N'].mean().sort_values(ascending=False)
    recall_by_algorithm.plot(kind='bar', color='skyblue')
    plt.title('Average Recall@N by Algorithm')
    plt.ylabel('Recall@N')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('recall_by_algorithm.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 2. BAR CHART: QPS ανά αλγόριθμο
    plt.figure(figsize=(10, 6))
    qps_by_algorithm = df.groupby('Algorithm')['QPS'].mean().sort_values(ascending=False)
    qps_by_algorithm.plot(kind='bar', color='lightgreen')
    plt.title('Average QPS by Algorithm')
    plt.ylabel('Queries Per Second')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('qps_by_algorithm.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 3. SCATTER PLOT: QPS vs Recall@N
    plt.figure(figsize=(10, 6))
    colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}

    for algo in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algo]
        plt.scatter(algo_data['QPS'], algo_data['Recall@N'],
                    c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)

    plt.xlabel('QPS (Queries Per Second)')
    plt.ylabel('Recall@N')
    plt.title('Trade-off: QPS vs Recall@N')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig('qps_vs_recall.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 4. SCATTER PLOT: Avg AF vs QPS
    plt.figure(figsize=(10, 6))
    for algo in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algo]
        plt.scatter(algo_data['QPS'], algo_data['Avg AF'],
                    c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)

    plt.xlabel('QPS (Queries Per Second)')
    plt.ylabel('Average Approximation Factor')
    plt.title('Trade-off: QPS vs Approximation Accuracy')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig('qps_vs_af.png', dpi=300, bbox_inches='tight')
    plt.close()

def analyze_parameter_effects(df):
    """Αναλύει την επίδραση των παραμέτρων"""

    # Για LSH: Επίδραση του L στο Recall
    lsh_data = df[df['Algorithm'] == 'LSH']
    if not lsh_data.empty and {'k', 'L', 'Recall@N'}.issubset(lsh_data.columns):
        plt.figure(figsize=(10, 6))
        for k_val in lsh_data['k'].unique():
            k_data = lsh_data[lsh_data['k'] == k_val]
            plt.plot(k_data['L'], k_data['Recall@N'], 'o-', label=f'k={k_val}')

        plt.xlabel('L (Number of hash tables)')
        plt.ylabel('Recall@N')
        plt.title('LSH: Effect of L on Recall for different k values')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig('lsh_L_vs_recall.png', dpi=300, bbox_inches='tight')
        plt.close()
    else:
        print("⚠️ Δεν βρέθηκαν στήλες 'k' ή 'L' για LSH — παράλειψη γραφήματος.")

    # Για IVFFlat: Επίδραση του nprobe
    ivf_data = df[df['Algorithm'] == 'IVFFlat']
    if not ivf_data.empty and {'nprobe', 'Recall@N', 'QPS'}.issubset(ivf_data.columns):
        plt.figure(figsize=(10, 6))
        plt.plot(ivf_data['nprobe'], ivf_data['Recall@N'], 'o-', color='green', label='Recall')
        plt.plot(ivf_data['nprobe'], ivf_data['QPS'], 's-', color='red', label='QPS')
        plt.xlabel('nprobe')
        plt.ylabel('Metric Value')
        plt.title('IVFFlat: Effect of nprobe on Recall and QPS')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig('ivfflat_nprobe_effect.png', dpi=300, bbox_inches='tight')
        plt.close()
    else:
        print("⚠️ Δεν βρέθηκε στήλη 'nprobe' για IVFFlat — παράλειψη γραφήματος.")

def generate_summary_statistics(df):
    """Δημιουργεί σύνοψη στατιστικών"""
    print("=" * 60)
    print("📈 SUMMARY STATISTICS")
    print("=" * 60)

    # Καλύτερος αλγόριθμος ανά μετρική
    best_recall = df.loc[df['Recall@N'].idxmax()]
    best_qps = df.loc[df['QPS'].idxmax()]
    best_af = df.loc[df['Avg AF'].idxmin()]

    print(f"🎯 Best Recall@N: {best_recall['Recall@N']:.3f} ({best_recall['Algorithm']} - {best_recall.get('Parameters', 'N/A')})")
    print(f"⚡ Best QPS: {best_qps['QPS']:.1f} ({best_qps['Algorithm']} - {best_qps.get('Parameters', 'N/A')})")
    print(f"📏 Best AF (closest to 1): {best_af['Avg AF']:.3f} ({best_af['Algorithm']} - {best_af.get('Parameters', 'N/A')})")

    print("\n📊 Average by Algorithm:")
    avg_by_algo = df.groupby('Algorithm')[['Recall@N', 'QPS', 'Avg AF']].mean()
    print(avg_by_algo.round(3))

def main_analysis():
    # Φόρτωση των δεδομένων
    df = pd.read_csv('algorithm_comparison_table.csv')
    print("📁 Columns found:", df.columns.tolist())

    # Δημιουργία γραφημάτων
    create_comparison_plots(df)
    analyze_parameter_effects(df)
    generate_summary_statistics(df)

    # Αποθήκευση αναλυτικού πίνακα
    if {'Algorithm', 'Dataset', 'Recall@N', 'QPS', 'Avg AF', 'Avg Approx Time (s)'}.issubset(df.columns):
        summary_table = df.groupby(['Algorithm', 'Dataset']).agg({
            'Recall@N': 'mean',
            'QPS': 'mean',
            'Avg AF': 'mean',
            'Avg Approx Time (s)': 'mean'
        }).round(3)
        summary_table.to_csv('detailed_summary.csv')
        print(f"\n✅ Saved detailed summary to detailed_summary.csv")
    else:
        print("⚠️ Κάποιες στήλες λείπουν — δεν δημιουργήθηκε detailed_summary.csv")
    


if __name__ == "__main__":
    main_analysis()
