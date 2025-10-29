import matplotlib
matplotlib.use('Agg')  # Απενεργοποιεί το GUI backend για να μην εμφανίζονται warnings
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np

def create_scatter_pairwise_comparisons(df):
    """Δημιουργεί ξεχωριστά scatter plots για κάθε ζεύγος μετρικών"""
    
    metrics_pairs = [
        ('Recall@N', 'QPS'),
        ('Recall@N', 'Avg AF'),
        ('Recall@N', 'Avg Approx Time (s)'),
        ('QPS', 'Avg AF'),
        ('QPS', 'Avg Approx Time (s)'),
        ('Avg AF', 'Avg Approx Time (s)'),
        ('Avg Approx Time (s)', 'Avg Exact Time (s)')
    ]
    
    colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
    
    for metric_x, metric_y in metrics_pairs:
        if metric_x in df.columns and metric_y in df.columns:
            plt.figure(figsize=(10, 6))
            
            for algo in df['Algorithm'].unique():
                algo_data = df[df['Algorithm'] == algo]
                plt.scatter(algo_data[metric_x], algo_data[metric_y],
                           c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)
            
            plt.xlabel(metric_x)
            plt.ylabel(metric_y)
            plt.title(f'{metric_y} vs {metric_x}')
            plt.legend()
            plt.grid(True, alpha=0.3)
            
            # Προσθήκη γραμμής τάσης
            try:
                z = np.polyfit(df[metric_x], df[metric_y], 1)
                p = np.poly1d(z)
                plt.plot(df[metric_x], p(df[metric_x]), "r--", alpha=0.8, 
                        label=f'Trend (ρ={df[metric_x].corr(df[metric_y]):.2f})')
                plt.legend()
            except:
                pass
            
            filename = f'comparison_{metric_x.replace(" ", "_").replace("(", "").replace(")", "")}_vs_{metric_y.replace(" ", "_").replace("(", "").replace(")", "")}.png'
            plt.tight_layout()
            plt.savefig(filename, dpi=300, bbox_inches='tight')
            plt.close()
            print(f"✅ Saved {filename}")

def create_bar_pairwise_comparisons(df):
    """Δημιουργεί ξεχωριστά bar charts για κάθε ζεύγος μετρικών - ΧΩΡΙΣ error bars"""
    
    metrics_pairs = [
        ('Recall@N', 'QPS'),
        ('Recall@N', 'Avg AF'),
        ('Recall@N', 'Avg Approx Time (s)'),
        ('QPS', 'Avg AF'),
        ('QPS', 'Avg Approx Time (s)'),
        ('Avg AF', 'Avg Approx Time (s)'),
        ('Avg Approx Time (s)', 'Avg Exact Time (s)')
    ]
    
    colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
    
    for metric_x, metric_y in metrics_pairs:
        if metric_x in df.columns and metric_y in df.columns:
            fig, ax = plt.subplots(figsize=(12, 8))
            
            algorithms = df['Algorithm'].unique()
            x_pos = np.arange(len(algorithms))
            width = 0.35
            
            # Υπολογισμός μέσων όρων για κάθε αλγόριθμο
            means_x = []
            means_y = []
            
            for algo in algorithms:
                algo_data = df[df['Algorithm'] == algo]
                means_x.append(algo_data[metric_x].mean())
                means_y.append(algo_data[metric_y].mean())
            
            # Δημιουργία ράβδων ΧΩΡΙΣ error bars
            bars1 = ax.bar(x_pos - width/2, means_x, width, 
                          label=metric_x, alpha=0.8, 
                          color='skyblue')  # ΑΦΑΙΡΕΣΑ το yerr=std_x
            
            bars2 = ax.bar(x_pos + width/2, means_y, width, 
                          label=metric_y, alpha=0.8, 
                          color='lightcoral')  # ΑΦΑΙΡΕΣΑ το yerr=std_y
            
            ax.set_xlabel('Algorithms')
            ax.set_ylabel('Metric Values')
            ax.set_title(f'Comparison: {metric_x} vs {metric_y} by Algorithm')
            ax.set_xticks(x_pos)
            ax.set_xticklabels(algorithms, rotation=45)
            ax.legend()
            
            # Απενεργοποίηση πλέγματος και πλαισίου
            ax.grid(False)
            ax.spines['top'].set_visible(False)
            ax.spines['right'].set_visible(False)
            
            # Προσθήκη τιμών πάνω από τις ράβδους
            for bars in [bars1, bars2]:
                for bar in bars:
                    height = bar.get_height()
                    ax.text(bar.get_x() + bar.get_width()/2., height + 0.01 * max(means_x + means_y),
                           f'{height:.3f}', ha='center', va='bottom', fontsize=8)
            
            plt.tight_layout()
            filename = f'bar_comparison_{metric_x.replace(" ", "_").replace("(", "").replace(")", "")}_vs_{metric_y.replace(" ", "_").replace("(", "").replace(")", "")}.png'
            plt.savefig(filename, dpi=300, bbox_inches='tight', 
                       pad_inches=0.1, facecolor='white', edgecolor='none')
            plt.close()
            print(f"✅ Saved {filename}")

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

    # 3. BAR CHART: AF ανά αλγόριθμο
    plt.figure(figsize=(10, 6))
    af_by_algorithm = df.groupby('Algorithm')['Avg AF'].mean().sort_values(ascending=False)
    af_by_algorithm.plot(kind='bar', color='lightcoral')
    plt.title('Average AF by Algorithm')
    plt.ylabel('Avg AF')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('af_by_algorithm.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 4. BAR CHART: Avg Approx Time (s) ανά αλγόριθμο
    plt.figure(figsize=(10, 6))
    approx_time_by_algorithm = df.groupby('Algorithm')['Avg Approx Time (s)'].mean().sort_values(ascending=False)
    approx_time_by_algorithm.plot(kind='bar', color='lightblue')
    plt.title('Avg Approx Time (s) by Algorithm')
    plt.ylabel('Avg Approx Time (s)')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('approx_time_by_algorithm.png', dpi=300, bbox_inches='tight')
    plt.close()

    
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
    create_scatter_pairwise_comparisons(df)
    create_comparison_plots(df)
    create_bar_pairwise_comparisons(df)      
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