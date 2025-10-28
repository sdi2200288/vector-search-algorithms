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
#     plt.show()

#     # 2. BAR CHART: QPS ανά αλγόριθμο
#     plt.figure(figsize=(10, 6))
#     qps_by_algorithm = df.groupby('Algorithm')['QPS'].mean().sort_values(ascending=False)
#     qps_by_algorithm.plot(kind='bar', color='lightgreen')
#     plt.title('Average QPS by Algorithm')
#     plt.ylabel('Queries Per Second')
#     plt.xticks(rotation=45)
#     plt.tight_layout()
#     plt.savefig('qps_by_algorithm.png', dpi=300, bbox_inches='tight')
#     plt.show()

#     # 3. SCATTER PLOT: QPS vs Recall@N
#     plt.figure(figsize=(10, 6))
#     colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
    
#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         plt.scatter(algo_data['QPS'], algo_data['Recall@N'], 
#                    c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)
    
#     plt.xlabel('QPS (Queries Per Second)')
#     plt.ylabel('Recall@N')
#     plt.title('Trade-off: QPS vs Recall@N')
#     plt.legend()
#     plt.grid(True, alpha=0.3)
#     plt.tight_layout()
#     plt.savefig('qps_vs_recall.png', dpi=300, bbox_inches='tight')
#     plt.show()

#     # 4. SCATTER PLOT: Avg AF vs QPS
#     plt.figure(figsize=(10, 6))
#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         plt.scatter(algo_data['QPS'], algo_data['Avg AF'], 
#                    c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)
    
#     plt.xlabel('QPS (Queries Per Second)')
#     plt.ylabel('Average Approximation Factor')
#     plt.title('Trade-off: QPS vs Approximation Accuracy')
#     plt.legend()
#     plt.grid(True, alpha=0.3)
#     plt.tight_layout()
#     plt.savefig('qps_vs_af.png', dpi=300, bbox_inches='tight')
#     plt.show()

# def analyze_parameter_effects(df):
#     """Αναλύει την επίδραση των παραμέτρων"""
    
#     # Για LSH: Επίδραση του L στο Recall
#     lsh_data = df[df['Algorithm'] == 'LSH']
#     if not lsh_data.empty:
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
#         plt.show()

#     # Για IVFFlat: Επίδραση του nprobe
#     ivf_data = df[df['Algorithm'] == 'IVFFlat']
#     if not ivf_data.empty:
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
#         plt.show()

# def generate_summary_statistics(df):
#     """Δημιουργεί σύνοψη στατιστικών"""
#     print("="*60)
#     print("📈 SUMMARY STATISTICS")
#     print("="*60)
    
#     # Καλύτερος αλγόριθμος ανά μετρική
#     best_recall = df.loc[df['Recall@N'].idxmax()]
#     best_qps = df.loc[df['QPS'].idxmax()]
#     best_af = df.loc[df['Avg AF'].idxmin()]
    
#     print(f"🎯 Best Recall@N: {best_recall['Recall@N']:.3f} ({best_recall['Algorithm']} - {best_recall['Parameters']})")
#     print(f"⚡ Best QPS: {best_qps['QPS']:.1f} ({best_qps['Algorithm']} - {best_qps['Parameters']})")
#     print(f"📏 Best AF (closest to 1): {best_af['Avg AF']:.3f} ({best_af['Algorithm']} - {best_af['Parameters']})")
    
#     print("\n📊 Average by Algorithm:")
#     avg_by_algo = df.groupby('Algorithm')[['Recall@N', 'QPS', 'Avg AF']].mean()
#     print(avg_by_algo.round(3))

# # Κύριο πρόγραμμα ανάλυσης
# def main_analysis():
#     # Φόρτωση των δεδομένων
#     df = pd.read_csv('algorithm_comparison_table.csv')
    
#     # Δημιουργία γραφημάτων
#     create_comparison_plots(df)
#     analyze_parameter_effects(df)
#     generate_summary_statistics(df)
    
#     # Αποθήκευση αναλυτικού πίνακα
#     summary_table = df.groupby(['Algorithm', 'Dataset']).agg({
#         'Recall@N': 'mean',
#         'QPS': 'mean', 
#         'Avg AF': 'mean',
#         'Avg Approx Time (s)': 'mean'
#     }).round(3)
    
#     summary_table.to_csv('detailed_summary.csv')
#     print(f"\n✅ Saved detailed summary to detailed_summary.csv")

# if __name__ == "__main__":
#     main_analysis()




import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import os

# Styling για όμορφα διαγράμματα
plt.style.use('default')
sns.set_palette("husl")
plt.rcParams['figure.figsize'] = (12, 8)
plt.rcParams['font.size'] = 12

def create_algorithm_comparison_plots(df):
    """Δημιουργία διαγραμμάτων σύγκρισης αλγορίθμων"""
    print("📊 Creating algorithm comparison plots...")
    
    fig, axes = plt.subplots(2, 3, figsize=(20, 12))
    fig.suptitle('Σύγκριση Αλγορίθμων Αναζήτησης - Τελικές Μετρικές', fontsize=16, fontweight='bold')
    
    # 1. Bar plot - Average AF
    af_data = df.groupby('Algorithm')['Avg_AF'].mean().sort_values()
    axes[0,0].bar(af_data.index, af_data.values, color='skyblue', alpha=0.7)
    axes[0,0].set_title('Μέσο Approximation Factor (AF)')
    axes[0,0].set_ylabel('AF (πλησιέστερο στο 1 = καλύτερο)')
    axes[0,0].tick_params(axis='x', rotation=45)
    
    # Προσθήκη τιμών στις μπάρες
    for i, v in enumerate(af_data.values):
        axes[0,0].text(i, v + 0.01, f'{v:.3f}', ha='center', va='bottom')
    
    # 2. Bar plot - Average Recall
    recall_data = df.groupby('Algorithm')['Avg_Recall'].mean().sort_values(ascending=False)
    axes[0,1].bar(recall_data.index, recall_data.values, color='lightgreen', alpha=0.7)
    axes[0,1].set_title('Μέσος Recall@N')
    axes[0,1].set_ylabel('Recall (υψηλότερο = καλύτερο)')
    axes[0,1].set_ylim(0, 1)
    axes[0,1].tick_params(axis='x', rotation=45)
    
    for i, v in enumerate(recall_data.values):
        axes[0,1].text(i, v + 0.01, f'{v:.3f}', ha='center', va='bottom')
    
    # 3. Bar plot - QPS
    qps_data = df.groupby('Algorithm')['QPS'].mean().sort_values(ascending=False)
    axes[0,2].bar(qps_data.index, qps_data.values, color='salmon', alpha=0.7)
    axes[0,2].set_title('Ταχύτητα (QPS)')
    axes[0,2].set_ylabel('Queries Per Second (υψηλότερο = καλύτερο)')
    axes[0,2].tick_params(axis='x', rotation=45)
    
    for i, v in enumerate(qps_data.values):
        axes[0,2].text(i, v + max(qps_data.values)*0.01, f'{v:.0f}', ha='center', va='bottom')
    
    # 4. Scatter plot - AF vs Recall
    colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
    for algo in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algo]
        axes[1,0].scatter(algo_data['Avg_AF'], algo_data['Avg_Recall'], 
                         label=algo, s=100, alpha=0.7, c=colors.get(algo, 'gray'))
    
    axes[1,0].set_xlabel('Average AF')
    axes[1,0].set_ylabel('Average Recall')
    axes[1,0].set_title('AF vs Recall')
    axes[1,0].legend()
    axes[1,0].grid(True, alpha=0.3)
    
    # 5. Scatter plot - Recall vs QPS
    for algo in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algo]
        axes[1,1].scatter(algo_data['Avg_Recall'], algo_data['QPS'], 
                         label=algo, s=100, alpha=0.7, c=colors.get(algo, 'gray'))
    
    axes[1,1].set_xlabel('Average Recall')
    axes[1,1].set_ylabel('QPS')
    axes[1,1].set_title('Recall vs Ταχύτητα')
    axes[1,1].legend()
    axes[1,1].grid(True, alpha=0.3)
    
    # 6. Χρόνοι αναζήτησης
    time_data = df[['Algorithm', 'Avg_Approx_Time', 'Avg_Exact_Time']].copy()
    time_melted = time_data.melt(id_vars=['Algorithm'], 
                                value_vars=['Avg_Approx_Time', 'Avg_Exact_Time'],
                                var_name='Time_Type', 
                                value_name='Time')
    
    # Χαρτογράφηση ονομάτων για καλύτερη εμφάνιση
    time_melted['Time_Type'] = time_melted['Time_Type'].map({
        'Avg_Approx_Time': 'Προσεγγιστική',
        'Avg_Exact_Time': 'Ακριβής'
    })
    
    sns.boxplot(data=time_melted, x='Algorithm', y='Time', hue='Time_Type', ax=axes[1,2])
    axes[1,2].set_title('Χρόνοι Αναζήτησης')
    axes[1,2].set_ylabel('Χρόνος (seconds)')
    axes[1,2].set_yscale('log')  # Log scale για καλύτερη οπτικοποίηση
    axes[1,2].tick_params(axis='x', rotation=45)
    
    plt.tight_layout()
    plt.savefig('algorithm_comparison.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    print("✅ Algorithm comparison plots saved!")

def create_parameter_sensitivity_plots(df):
    """Δημιουργία διαγραμμάτων ευαισθησίας παραμέτρων"""
    print("📈 Creating parameter sensitivity plots...")
    
    # LSH Parameter Analysis
    lsh_data = df[df['Algorithm'] == 'LSH']
    if not lsh_data.empty and 'k' in lsh_data.columns:
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle('Ευαισθησία Παραμέτρων LSH', fontsize=16, fontweight='bold')
        
        # k vs AF & Recall
        if 'k' in lsh_data.columns:
            axes[0,0].plot(lsh_data['k'], lsh_data['Avg_AF'], 'o-', linewidth=2, markersize=8, label='AF')
            axes[0,0].set_xlabel('k (Hash Functions)')
            axes[0,0].set_ylabel('AF', color='blue')
            axes[0,0].tick_params(axis='y', labelcolor='blue')
            
            ax2 = axes[0,0].twinx()
            ax2.plot(lsh_data['k'], lsh_data['Avg_Recall'], 's-', color='red', linewidth=2, markersize=6, label='Recall')
            ax2.set_ylabel('Recall', color='red')
            ax2.tick_params(axis='y', labelcolor='red')
            ax2.set_ylim(0, 1)
            
            axes[0,0].set_title('AF & Recall vs k')
            axes[0,0].grid(True, alpha=0.3)
        
        # L vs AF & Recall
        if 'L' in lsh_data.columns:
            axes[0,1].plot(lsh_data['L'], lsh_data['Avg_AF'], 'o-', linewidth=2, markersize=8, label='AF')
            axes[0,1].set_xlabel('L (Hash Tables)')
            axes[0,1].set_ylabel('AF', color='blue')
            axes[0,1].tick_params(axis='y', labelcolor='blue')
            
            ax2 = axes[0,1].twinx()
            ax2.plot(lsh_data['L'], lsh_data['Avg_Recall'], 's-', color='red', linewidth=2, markersize=6, label='Recall')
            ax2.set_ylabel('Recall', color='red')
            ax2.tick_params(axis='y', labelcolor='red')
            ax2.set_ylim(0, 1)
            
            axes[0,1].set_title('AF & Recall vs L')
            axes[0,1].grid(True, alpha=0.3)
        
        # k vs QPS
        if 'k' in lsh_data.columns:
            axes[1,0].plot(lsh_data['k'], lsh_data['QPS'], 'o-', linewidth=2, markersize=8, color='green')
            axes[1,0].set_xlabel('k (Hash Functions)')
            axes[1,0].set_ylabel('QPS')
            axes[1,0].set_title('Ταχύτητα (QPS) vs k')
            axes[1,0].grid(True, alpha=0.3)
        
        # L vs QPS
        if 'L' in lsh_data.columns:
            axes[1,1].plot(lsh_data['L'], lsh_data['QPS'], 'o-', linewidth=2, markersize=8, color='green')
            axes[1,1].set_xlabel('L (Hash Tables)')
            axes[1,1].set_ylabel('QPS')
            axes[1,1].set_title('Ταχύτητα (QPS) vs L')
            axes[1,1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig('lsh_parameter_sensitivity.png', dpi=300, bbox_inches='tight')
        plt.show()
        print("✅ LSH parameter sensitivity plots saved!")

def create_correlation_analysis(df):
    """Ανάλυση συσχέτισης μεταξύ μετρικών"""
    print("🔗 Creating correlation analysis...")
    
    # Select numeric columns for correlation
    numeric_cols = ['Avg_AF', 'Avg_Recall', 'QPS', 'Avg_Approx_Time', 'Avg_Exact_Time']
    numeric_df = df[numeric_cols].select_dtypes(include=[np.number])
    
    # Correlation matrix
    corr_matrix = numeric_df.corr()
    
    plt.figure(figsize=(10, 8))
    mask = np.triu(np.ones_like(corr_matrix, dtype=bool))  # Μask για πάνω τρίγωνο
    sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0,
                square=True, fmt='.2f', cbar_kws={'shrink': 0.8}, mask=mask)
    plt.title('Μητρώο Συσχέτισης Μετρικών Απόδοσης', fontsize=16, fontweight='bold')
    plt.tight_layout()
    plt.savefig('correlation_matrix.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # AF vs Recall scatter plot με regression line
    plt.figure(figsize=(10, 6))
    for algo in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algo]
        plt.scatter(algo_data['Avg_AF'], algo_data['Avg_Recall'], 
                   label=algo, s=80, alpha=0.7)
    
    # Προσθήκη regression line
    z = np.polyfit(df['Avg_AF'], df['Avg_Recall'], 1)
    p = np.poly1d(z)
    plt.plot(df['Avg_AF'], p(df['Avg_AF']), "r--", alpha=0.8, label='Regression Line')
    
    plt.xlabel('Average Approximation Factor (AF)')
    plt.ylabel('Average Recall@N')
    plt.title('Συσχέτιση AF και Recall')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig('af_vs_recall_correlation.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    print("✅ Correlation analysis plots saved!")

def create_dataset_comparison(df):
    """Σύγκριση απόδοσης μεταξύ datasets"""
    if 'Dataset' in df.columns and df['Dataset'].nunique() > 1:
        print("📁 Creating dataset comparison plots...")
        
        dataset_data = df.groupby(['Algorithm', 'Dataset']).agg({
            'Avg_AF': 'mean',
            'Avg_Recall': 'mean',
            'QPS': 'mean'
        }).reset_index()
        
        fig, axes = plt.subplots(1, 3, figsize=(18, 6))
        fig.suptitle('Σύγκριση Απόδοσης Ανά Dataset', fontsize=16, fontweight='bold')
        
        # AF ανά dataset
        sns.barplot(data=dataset_data, x='Algorithm', y='Avg_AF', hue='Dataset', ax=axes[0])
        axes[0].set_title('Μέσο AF Ανά Dataset')
        axes[0].set_ylabel('AF')
        axes[0].tick_params(axis='x', rotation=45)
        
        # Recall ανά dataset
        sns.barplot(data=dataset_data, x='Algorithm', y='Avg_Recall', hue='Dataset', ax=axes[1])
        axes[1].set_title('Μέσο Recall Ανά Dataset')
        axes[1].set_ylabel('Recall')
        axes[1].tick_params(axis='x', rotation=45)
        
        # QPS ανά dataset
        sns.barplot(data=dataset_data, x='Algorithm', y='QPS', hue='Dataset', ax=axes[2])
        axes[2].set_title('Ταχύτητα (QPS) Ανά Dataset')
        axes[2].set_ylabel('QPS')
        axes[2].tick_params(axis='x', rotation=45)
        
        plt.tight_layout()
        plt.savefig('dataset_comparison.png', dpi=300, bbox_inches='tight')
        plt.show()
        print("✅ Dataset comparison plots saved!")

def main():
    """Κύρια συνάρτηση δημιουργίας διαγραμμάτων"""
    try:
        # Διάβασε τα αποτελέσματα από το νέο parser
        df = pd.read_csv('final_metrics_comparison.csv')
        print(f"📊 Loaded {len(df)} experiment results from final_metrics_comparison.csv")
        
        # Δημιουργία όλων των διαγραμμάτων
        create_algorithm_comparison_plots(df)
        create_parameter_sensitivity_plots(df)
        create_correlation_analysis(df)
        create_dataset_comparison(df)
        
        print("\n🎉 All plots generated successfully!")
        print("📁 Generated files:")
        print("   - algorithm_comparison.png")
        print("   - lsh_parameter_sensitivity.png") 
        print("   - correlation_matrix.png")
        print("   - af_vs_recall_correlation.png")
        print("   - dataset_comparison.png (if multiple datasets)")
        
    except FileNotFoundError:
        print("❌ final_metrics_comparison.csv not found. Run parse_results.py first.")
    except Exception as e:
        print(f"❌ Error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()