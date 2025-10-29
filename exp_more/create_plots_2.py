# import matplotlib
# matplotlib.use('Agg')  # Απενεργοποιεί το GUI backend για να μην εμφανίζονται warnings

# import matplotlib.pyplot as plt
# import seaborn as sns
# import pandas as pd

# def create_comparison_plots(df):
#     """Δημιουργεί γραφήματα σύγκρισης"""

#     # 1. BAR CHART: Recall@N ανά αλγόριθμο
#     plt.figure(figsize=(10, 6))
#     recall_by_algorithm = df.groupby('Algorithm')['Recall@N'].mean().sort_values(ascending=False)
#     recall_by_algorithm.plot(kind='bar', color='skyblue')
#     plt.title('Average Recall@N by Algorithm')
#     plt.ylabel('Recall@N')
#     plt.xticks(rotation=45)
#     plt.tight_layout()
#     plt.savefig('recall_by_algorithm.png', dpi=300, bbox_inches='tight')
#     plt.close()

#     # 2. BAR CHART: QPS ανά αλγόριθμο
#     plt.figure(figsize=(10, 6))
#     qps_by_algorithm = df.groupby('Algorithm')['QPS'].mean().sort_values(ascending=False)
#     qps_by_algorithm.plot(kind='bar', color='lightgreen')
#     plt.title('Average QPS by Algorithm')
#     plt.ylabel('Queries Per Second')
#     plt.xticks(rotation=45)
#     plt.tight_layout()
#     plt.savefig('qps_by_algorithm.png', dpi=300, bbox_inches='tight')
#     plt.close()

#     # 3. SCATTER PLOT: QPS vs Recall@N
#     plt.figure(figsize=(10, 6))
#     colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}

#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         plt.scatter(algo_data['QPS'], algo_data['Recall@N'],
#                     c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)

#     plt.xlabel('QPS (Queries Per Second)')
#     plt.ylabel('Recall@N')
#     plt.title('Trade-off: QPS vs Recall@N')
#     plt.legend()
#     plt.grid(True, alpha=0.3)
#     plt.tight_layout()
#     plt.savefig('qps_vs_recall.png', dpi=300, bbox_inches='tight')
#     plt.close()

#     # 4. SCATTER PLOT: Avg AF vs QPS
#     plt.figure(figsize=(10, 6))
#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         plt.scatter(algo_data['QPS'], algo_data['Avg AF'],
#                     c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)

#     plt.xlabel('QPS (Queries Per Second)')
#     plt.ylabel('Average Approximation Factor')
#     plt.title('Trade-off: QPS vs Approximation Accuracy')
#     plt.legend()
#     plt.grid(True, alpha=0.3)
#     plt.tight_layout()
#     plt.savefig('qps_vs_af.png', dpi=300, bbox_inches='tight')
#     plt.close()

# def analyze_parameter_effects(df):
#     """Αναλύει την επίδραση των παραμέτρων"""

#     # Για LSH: Επίδραση του L στο Recall
#     lsh_data = df[df['Algorithm'] == 'LSH']
#     if not lsh_data.empty and {'k', 'L', 'Recall@N'}.issubset(lsh_data.columns):
#         plt.figure(figsize=(10, 6))
#         for k_val in lsh_data['k'].unique():
#             k_data = lsh_data[lsh_data['k'] == k_val]
#             plt.plot(k_data['L'], k_data['Recall@N'], 'o-', label=f'k={k_val}')

#         plt.xlabel('L (Number of hash tables)')
#         plt.ylabel('Recall@N')
#         plt.title('LSH: Effect of L on Recall for different k values')
#         plt.legend()
#         plt.grid(True, alpha=0.3)
#         plt.tight_layout()
#         plt.savefig('lsh_L_vs_recall.png', dpi=300, bbox_inches='tight')
#         plt.close()
#     else:
#         print("⚠️ Δεν βρέθηκαν στήλες 'k' ή 'L' για LSH — παράλειψη γραφήματος.")

#     # Για IVFFlat: Επίδραση του nprobe
#     ivf_data = df[df['Algorithm'] == 'IVFFlat']
#     if not ivf_data.empty and {'nprobe', 'Recall@N', 'QPS'}.issubset(ivf_data.columns):
#         plt.figure(figsize=(10, 6))
#         plt.plot(ivf_data['nprobe'], ivf_data['Recall@N'], 'o-', color='green', label='Recall')
#         plt.plot(ivf_data['nprobe'], ivf_data['QPS'], 's-', color='red', label='QPS')
#         plt.xlabel('nprobe')
#         plt.ylabel('Metric Value')
#         plt.title('IVFFlat: Effect of nprobe on Recall and QPS')
#         plt.legend()
#         plt.grid(True, alpha=0.3)
#         plt.tight_layout()
#         plt.savefig('ivfflat_nprobe_effect.png', dpi=300, bbox_inches='tight')
#         plt.close()
#     else:
#         print("⚠️ Δεν βρέθηκε στήλη 'nprobe' για IVFFlat — παράλειψη γραφήματος.")

# def generate_summary_statistics(df):
#     """Δημιουργεί σύνοψη στατιστικών"""
#     print("=" * 60)
#     print("📈 SUMMARY STATISTICS")
#     print("=" * 60)

#     # Καλύτερος αλγόριθμος ανά μετρική
#     best_recall = df.loc[df['Recall@N'].idxmax()]
#     best_qps = df.loc[df['QPS'].idxmax()]
#     best_af = df.loc[df['Avg AF'].idxmin()]

#     print(f"🎯 Best Recall@N: {best_recall['Recall@N']:.3f} ({best_recall['Algorithm']} - {best_recall.get('Parameters', 'N/A')})")
#     print(f"⚡ Best QPS: {best_qps['QPS']:.1f} ({best_qps['Algorithm']} - {best_qps.get('Parameters', 'N/A')})")
#     print(f"📏 Best AF (closest to 1): {best_af['Avg AF']:.3f} ({best_af['Algorithm']} - {best_af.get('Parameters', 'N/A')})")

#     print("\n📊 Average by Algorithm:")
#     avg_by_algo = df.groupby('Algorithm')[['Recall@N', 'QPS', 'Avg AF']].mean()
#     print(avg_by_algo.round(3))

# def main_analysis():
#     # Φόρτωση των δεδομένων
#     df = pd.read_csv('algorithm_comparison_table.csv')
#     print("📁 Columns found:", df.columns.tolist())

#     # Δημιουργία γραφημάτων
#     create_comparison_plots(df)
#     analyze_parameter_effects(df)
#     generate_summary_statistics(df)

#     # Αποθήκευση αναλυτικού πίνακα
#     if {'Algorithm', 'Dataset', 'Recall@N', 'QPS', 'Avg AF', 'Avg Approx Time (s)'}.issubset(df.columns):
#         summary_table = df.groupby(['Algorithm', 'Dataset']).agg({
#             'Recall@N': 'mean',
#             'QPS': 'mean',
#             'Avg AF': 'mean',
#             'Avg Approx Time (s)': 'mean'
#         }).round(3)
#         summary_table.to_csv('detailed_summary.csv')
#         print(f"\n✅ Saved detailed summary to detailed_summary.csv")
#     else:
#         print("⚠️ Κάποιες στήλες λείπουν — δεν δημιουργήθηκε detailed_summary.csv")
    


# if __name__ == "__main__":
#     main_analysis()

import matplotlib
matplotlib.use('Agg')  # Απενεργοποιεί το GUI backend για να μην εμφανίζονται warnings

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np

def create_metric_comparison_matrix(df):
    """Δημιουργεί πλέγμα με όλες τις πιθανές συγκρίσεις μεταξύ μετρικών"""
    
    # Λίστα με όλες τις μετρικές για σύγκριση
    metrics = ['Recall@N', 'QPS', 'Avg AF', 'Avg Approx Time (s)', 'Avg Exact Time (s)']
    
    # Φιλτράρουμε μόνο τις μετρικές που υπάρχουν στο dataframe
    available_metrics = [metric for metric in metrics if metric in df.columns]
    
    if len(available_metrics) < 2:
        print("⚠️ Δεν υπάρχουν αρκετές μετρικές για σύγκριση")
        return
    
    n_metrics = len(available_metrics)
    
    # Δημιουργούμε ένα πλέγμα scatter plots
    fig, axes = plt.subplots(n_metrics, n_metrics, figsize=(20, 20))
    fig.suptitle('Μήτρα Σύγκρισης Μετρικών Απόδοσης', fontsize=16, fontweight='bold')
    
    colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
    
    for i, metric_y in enumerate(available_metrics):
        for j, metric_x in enumerate(available_metrics):
            ax = axes[i, j]
            
            if i == j:
                # Διαγώνιος - histogram για κάθε μετρική
                for algo in df['Algorithm'].unique():
                    algo_data = df[df['Algorithm'] == algo]
                    ax.hist(algo_data[metric_x], alpha=0.6, label=algo, 
                           color=colors.get(algo, 'gray'), bins=10)
                ax.set_title(f'Distribution of {metric_x}')
                ax.legend(fontsize=8)
            else:
                # Scatter plot για σύγκριση μετρικών
                for algo in df['Algorithm'].unique():
                    algo_data = df[df['Algorithm'] == algo]
                    ax.scatter(algo_data[metric_x], algo_data[metric_y],
                              c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=60)
                
                ax.set_xlabel(metric_x)
                ax.set_ylabel(metric_y)
                
                # Προσθήκη correlation coefficient
                correlation = df[metric_x].corr(df[metric_y])
                ax.text(0.05, 0.95, f'ρ = {correlation:.2f}', 
                       transform=ax.transAxes, fontsize=10,
                       bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.8))
            
            ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('metric_comparison_matrix.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("✅ Saved metric comparison matrix to metric_comparison_matrix.png")

def create_pairwise_comparisons(df):
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

def create_radar_chart(df):
    """Δημιουργεί radar chart για οπτική σύγκριση αλγορίθμων"""
    
    try:
        # Επιλογή και κανονικοποίηση μετρικών
        metrics = ['Recall@N', 'QPS', 'Avg AF']
        available_metrics = [m for m in metrics if m in df.columns]
        
        if len(available_metrics) < 3:
            print("⚠️ Δεν υπάρχουν αρκετές μετρικές για radar chart")
            return
        
        # Υπολογισμός μέσων όρων ανά αλγόριθμο
        algo_means = df.groupby('Algorithm')[available_metrics].mean()
        
        # Κανονικοποίηση (0-1) - για AF θέλουμε το αντίστροφο (μικρότερο = καλύτερο)
        normalized = algo_means.copy()
        for metric in available_metrics:
            if metric == 'Avg AF':
                # Για AF, το μικρότερο είναι καλύτερο
                normalized[metric] = 1 - (algo_means[metric] - algo_means[metric].min()) / (algo_means[metric].max() - algo_means[metric].min())
            else:
                # Για Recall και QPS, το μεγαλύτερο είναι καλύτερο
                normalized[metric] = (algo_means[metric] - algo_means[metric].min()) / (algo_means[metric].max() - algo_means[metric].min())
        
        # Δημιουργία radar chart
        angles = np.linspace(0, 2*np.pi, len(available_metrics), endpoint=False).tolist()
        angles += angles[:1]  # Κλείνουμε τον κύκλο
        
        fig, ax = plt.subplots(figsize=(10, 10), subplot_kw=dict(projection='polar'))
        
        colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
        
        for algo in normalized.index:
            values = normalized.loc[algo].values.tolist()
            values += values[:1]  # Κλείνουμε τον κύκλο
            ax.plot(angles, values, 'o-', linewidth=2, label=algo, color=colors.get(algo, 'gray'))
            ax.fill(angles, values, alpha=0.1, color=colors.get(algo, 'gray'))
        
        ax.set_xticks(angles[:-1])
        ax.set_xticklabels(available_metrics)
        ax.set_ylim(0, 1)
        plt.title('Radar Chart: Σύγκριση Αλγορίθμων (Κανονικοποιημένο 0-1)')
        plt.legend(loc='upper right', bbox_to_anchor=(1.3, 1.0))
        plt.tight_layout()
        plt.savefig('radar_chart_comparison.png', dpi=300, bbox_inches='tight')
        plt.close()
        print("✅ Saved radar chart to radar_chart_comparison.png")
        
    except Exception as e:
        print(f"⚠️ Error creating radar chart: {e}")

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

    # # 3. SCATTER PLOT: QPS vs Recall@N
    # plt.figure(figsize=(10, 6))
    # colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}

    # for algo in df['Algorithm'].unique():
    #     algo_data = df[df['Algorithm'] == algo]
    #     plt.scatter(algo_data['QPS'], algo_data['Recall@N'],
    #                 c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)

    # plt.xlabel('QPS (Queries Per Second)')
    # plt.ylabel('Recall@N')
    # plt.title('Trade-off: QPS vs Recall@N')
    # plt.legend()
    # plt.grid(True, alpha=0.3)
    # plt.tight_layout()
    # plt.savefig('qps_vs_recall.png', dpi=300, bbox_inches='tight')
    # plt.close()

    # # 4. SCATTER PLOT: Avg AF vs QPS
    # plt.figure(figsize=(10, 6))
    # for algo in df['Algorithm'].unique():
    #     algo_data = df[df['Algorithm'] == algo]
    #     plt.scatter(algo_data['QPS'], algo_data['Avg AF'],
    #                 c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)

    # plt.xlabel('QPS (Queries Per Second)')
    # plt.ylabel('Average Approximation Factor')
    # plt.title('Trade-off: QPS vs Approximation Accuracy')
    # plt.legend()
    # plt.grid(True, alpha=0.3)
    # plt.tight_layout()
    # plt.savefig('qps_vs_af.png', dpi=300, bbox_inches='tight')
    # plt.close()

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
    # create_metric_comparison_matrix(df)  # Νέο function
    create_pairwise_comparisons(df)      # Νέο function
    # create_radar_chart(df)               # Νέο function
    # analyze_parameter_effects(df)
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