# import pandas as pd
# import matplotlib.pyplot as plt
# import seaborn as sns
# import numpy as np
# import os

# # Styling για όμορφα διαγράμματα
# plt.style.use('default')
# sns.set_palette("husl")
# plt.rcParams['figure.figsize'] = (12, 8)
# plt.rcParams['font.size'] = 12

# def create_algorithm_comparison_plots(df):
#     """Δημιουργία διαγραμμάτων σύγκρισης αλγορίθμων"""
#     print("📊 Creating algorithm comparison plots...")
    
#     fig, axes = plt.subplots(2, 3, figsize=(20, 12))
#     fig.suptitle('Σύγκριση Αλγορίθμων Αναζήτησης - Τελικές Μετρικές', fontsize=16, fontweight='bold')
    
#     # 1. Bar plot - Average AF
#     af_data = df.groupby('Algorithm')['Avg_AF'].mean().sort_values()
#     axes[0,0].bar(af_data.index, af_data.values, color='skyblue', alpha=0.7)
#     axes[0,0].set_title('Μέσο Approximation Factor (AF)')
#     axes[0,0].set_ylabel('AF (πλησιέστερο στο 1 = καλύτερο)')
#     axes[0,0].tick_params(axis='x', rotation=45)
    
#     # Προσθήκη τιμών στις μπάρες
#     for i, v in enumerate(af_data.values):
#         axes[0,0].text(i, v + 0.01, f'{v:.3f}', ha='center', va='bottom')
    
#     # 2. Bar plot - Average Recall
#     recall_data = df.groupby('Algorithm')['Avg_Recall'].mean().sort_values(ascending=False)
#     axes[0,1].bar(recall_data.index, recall_data.values, color='lightgreen', alpha=0.7)
#     axes[0,1].set_title('Μέσος Recall@N')
#     axes[0,1].set_ylabel('Recall (υψηλότερο = καλύτερο)')
#     axes[0,1].set_ylim(0, 1)
#     axes[0,1].tick_params(axis='x', rotation=45)
    
#     for i, v in enumerate(recall_data.values):
#         axes[0,1].text(i, v + 0.01, f'{v:.3f}', ha='center', va='bottom')
    
#     # 3. Bar plot - QPS
#     qps_data = df.groupby('Algorithm')['QPS'].mean().sort_values(ascending=False)
#     axes[0,2].bar(qps_data.index, qps_data.values, color='salmon', alpha=0.7)
#     axes[0,2].set_title('Ταχύτητα (QPS)')
#     axes[0,2].set_ylabel('Queries Per Second (υψηλότερο = καλύτερο)')
#     axes[0,2].tick_params(axis='x', rotation=45)
    
#     for i, v in enumerate(qps_data.values):
#         axes[0,2].text(i, v + max(qps_data.values)*0.01, f'{v:.0f}', ha='center', va='bottom')
    
#     # 4. Scatter plot - AF vs Recall
#     colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         axes[1,0].scatter(algo_data['Avg_AF'], algo_data['Avg_Recall'], 
#                          label=algo, s=100, alpha=0.7, c=colors.get(algo, 'gray'))
    
#     axes[1,0].set_xlabel('Average AF')
#     axes[1,0].set_ylabel('Average Recall')
#     axes[1,0].set_title('AF vs Recall')
#     axes[1,0].legend()
#     axes[1,0].grid(True, alpha=0.3)
    
#     # 5. Scatter plot - Recall vs QPS
#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         axes[1,1].scatter(algo_data['Avg_Recall'], algo_data['QPS'], 
#                          label=algo, s=100, alpha=0.7, c=colors.get(algo, 'gray'))
    
#     axes[1,1].set_xlabel('Average Recall')
#     axes[1,1].set_ylabel('QPS')
#     axes[1,1].set_title('Recall vs Ταχύτητα')
#     axes[1,1].legend()
#     axes[1,1].grid(True, alpha=0.3)
    
#     # 6. Χρόνοι αναζήτησης
#     time_data = df[['Algorithm', 'Avg_Approx_Time', 'Avg_Exact_Time']].copy()
#     time_melted = time_data.melt(id_vars=['Algorithm'], 
#                                 value_vars=['Avg_Approx_Time', 'Avg_Exact_Time'],
#                                 var_name='Time_Type', 
#                                 value_name='Time')
    
#     # Χαρτογράφηση ονομάτων για καλύτερη εμφάνιση
#     time_melted['Time_Type'] = time_melted['Time_Type'].map({
#         'Avg_Approx_Time': 'Προσεγγιστική',
#         'Avg_Exact_Time': 'Ακριβής'
#     })
    
#     sns.boxplot(data=time_melted, x='Algorithm', y='Time', hue='Time_Type', ax=axes[1,2])
#     axes[1,2].set_title('Χρόνοι Αναζήτησης')
#     axes[1,2].set_ylabel('Χρόνος (seconds)')
#     axes[1,2].set_yscale('log')  # Log scale για καλύτερη οπτικοποίηση
#     axes[1,2].tick_params(axis='x', rotation=45)
    
#     plt.tight_layout()
#     plt.savefig('algorithm_comparison.png', dpi=300, bbox_inches='tight')
#     plt.show()
    
#     print("✅ Algorithm comparison plots saved!")

# def create_parameter_sensitivity_plots(df):
#     """Δημιουργία διαγραμμάτων ευαισθησίας παραμέτρων"""
#     print("📈 Creating parameter sensitivity plots...")
    
#     # LSH Parameter Analysis
#     lsh_data = df[df['Algorithm'] == 'LSH']
#     if not lsh_data.empty and 'k' in lsh_data.columns:
#         fig, axes = plt.subplots(2, 2, figsize=(15, 10))
#         fig.suptitle('Ευαισθησία Παραμέτρων LSH', fontsize=16, fontweight='bold')
        
#         # k vs AF & Recall
#         if 'k' in lsh_data.columns:
#             axes[0,0].plot(lsh_data['k'], lsh_data['Avg_AF'], 'o-', linewidth=2, markersize=8, label='AF')
#             axes[0,0].set_xlabel('k (Hash Functions)')
#             axes[0,0].set_ylabel('AF', color='blue')
#             axes[0,0].tick_params(axis='y', labelcolor='blue')
            
#             ax2 = axes[0,0].twinx()
#             ax2.plot(lsh_data['k'], lsh_data['Avg_Recall'], 's-', color='red', linewidth=2, markersize=6, label='Recall')
#             ax2.set_ylabel('Recall', color='red')
#             ax2.tick_params(axis='y', labelcolor='red')
#             ax2.set_ylim(0, 1)
            
#             axes[0,0].set_title('AF & Recall vs k')
#             axes[0,0].grid(True, alpha=0.3)
        
#         # L vs AF & Recall
#         if 'L' in lsh_data.columns:
#             axes[0,1].plot(lsh_data['L'], lsh_data['Avg_AF'], 'o-', linewidth=2, markersize=8, label='AF')
#             axes[0,1].set_xlabel('L (Hash Tables)')
#             axes[0,1].set_ylabel('AF', color='blue')
#             axes[0,1].tick_params(axis='y', labelcolor='blue')
            
#             ax2 = axes[0,1].twinx()
#             ax2.plot(lsh_data['L'], lsh_data['Avg_Recall'], 's-', color='red', linewidth=2, markersize=6, label='Recall')
#             ax2.set_ylabel('Recall', color='red')
#             ax2.tick_params(axis='y', labelcolor='red')
#             ax2.set_ylim(0, 1)
            
#             axes[0,1].set_title('AF & Recall vs L')
#             axes[0,1].grid(True, alpha=0.3)
        
#         # k vs QPS
#         if 'k' in lsh_data.columns:
#             axes[1,0].plot(lsh_data['k'], lsh_data['QPS'], 'o-', linewidth=2, markersize=8, color='green')
#             axes[1,0].set_xlabel('k (Hash Functions)')
#             axes[1,0].set_ylabel('QPS')
#             axes[1,0].set_title('Ταχύτητα (QPS) vs k')
#             axes[1,0].grid(True, alpha=0.3)
        
#         # L vs QPS
#         if 'L' in lsh_data.columns:
#             axes[1,1].plot(lsh_data['L'], lsh_data['QPS'], 'o-', linewidth=2, markersize=8, color='green')
#             axes[1,1].set_xlabel('L (Hash Tables)')
#             axes[1,1].set_ylabel('QPS')
#             axes[1,1].set_title('Ταχύτητα (QPS) vs L')
#             axes[1,1].grid(True, alpha=0.3)
        
#         plt.tight_layout()
#         plt.savefig('lsh_parameter_sensitivity.png', dpi=300, bbox_inches='tight')
#         plt.show()
#         print("✅ LSH parameter sensitivity plots saved!")

# def create_correlation_analysis(df):
#     """Ανάλυση συσχέτισης μεταξύ μετρικών"""
#     print("🔗 Creating correlation analysis...")
    
#     # Select numeric columns for correlation
#     numeric_cols = ['Avg_AF', 'Avg_Recall', 'QPS', 'Avg_Approx_Time', 'Avg_Exact_Time']
#     numeric_df = df[numeric_cols].select_dtypes(include=[np.number])
    
#     # Correlation matrix
#     corr_matrix = numeric_df.corr()
    
#     plt.figure(figsize=(10, 8))
#     mask = np.triu(np.ones_like(corr_matrix, dtype=bool))  # Μask για πάνω τρίγωνο
#     sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0,
#                 square=True, fmt='.2f', cbar_kws={'shrink': 0.8}, mask=mask)
#     plt.title('Μητρώο Συσχέτισης Μετρικών Απόδοσης', fontsize=16, fontweight='bold')
#     plt.tight_layout()
#     plt.savefig('correlation_matrix.png', dpi=300, bbox_inches='tight')
#     plt.show()
    
#     # AF vs Recall scatter plot με regression line
#     plt.figure(figsize=(10, 6))
#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         plt.scatter(algo_data['Avg_AF'], algo_data['Avg_Recall'], 
#                    label=algo, s=80, alpha=0.7)
    
#     # Προσθήκη regression line
#     z = np.polyfit(df['Avg_AF'], df['Avg_Recall'], 1)
#     p = np.poly1d(z)
#     plt.plot(df['Avg_AF'], p(df['Avg_AF']), "r--", alpha=0.8, label='Regression Line')
    
#     plt.xlabel('Average Approximation Factor (AF)')
#     plt.ylabel('Average Recall@N')
#     plt.title('Συσχέτιση AF και Recall')
#     plt.legend()
#     plt.grid(True, alpha=0.3)
#     plt.savefig('af_vs_recall_correlation.png', dpi=300, bbox_inches='tight')
#     plt.show()
    
#     print("✅ Correlation analysis plots saved!")

# def create_dataset_comparison(df):
#     """Σύγκριση απόδοσης μεταξύ datasets"""
#     if 'Dataset' in df.columns and df['Dataset'].nunique() > 1:
#         print("📁 Creating dataset comparison plots...")
        
#         dataset_data = df.groupby(['Algorithm', 'Dataset']).agg({
#             'Avg_AF': 'mean',
#             'Avg_Recall': 'mean',
#             'QPS': 'mean'
#         }).reset_index()
        
#         fig, axes = plt.subplots(1, 3, figsize=(18, 6))
#         fig.suptitle('Σύγκριση Απόδοσης Ανά Dataset', fontsize=16, fontweight='bold')
        
#         # AF ανά dataset
#         sns.barplot(data=dataset_data, x='Algorithm', y='Avg_AF', hue='Dataset', ax=axes[0])
#         axes[0].set_title('Μέσο AF Ανά Dataset')
#         axes[0].set_ylabel('AF')
#         axes[0].tick_params(axis='x', rotation=45)
        
#         # Recall ανά dataset
#         sns.barplot(data=dataset_data, x='Algorithm', y='Avg_Recall', hue='Dataset', ax=axes[1])
#         axes[1].set_title('Μέσο Recall Ανά Dataset')
#         axes[1].set_ylabel('Recall')
#         axes[1].tick_params(axis='x', rotation=45)
        
#         # QPS ανά dataset
#         sns.barplot(data=dataset_data, x='Algorithm', y='QPS', hue='Dataset', ax=axes[2])
#         axes[2].set_title('Ταχύτητα (QPS) Ανά Dataset')
#         axes[2].set_ylabel('QPS')
#         axes[2].tick_params(axis='x', rotation=45)
        
#         plt.tight_layout()
#         plt.savefig('dataset_comparison.png', dpi=300, bbox_inches='tight')
#         plt.show()
#         print("✅ Dataset comparison plots saved!")

# def main():
#     """Κύρια συνάρτηση δημιουργίας διαγραμμάτων"""
#     try:
#         # Διάβασε τα αποτελέσματα από το νέο parser
#         df = pd.read_csv('final_metrics_comparison.csv')
#         print(f"📊 Loaded {len(df)} experiment results from final_metrics_comparison.csv")
        
#         # Δημιουργία όλων των διαγραμμάτων
#         create_algorithm_comparison_plots(df)
#         create_parameter_sensitivity_plots(df)
#         create_correlation_analysis(df)
#         create_dataset_comparison(df)
        
#         print("\n🎉 All plots generated successfully!")
#         print("📁 Generated files:")
#         print("   - algorithm_comparison.png")
#         print("   - lsh_parameter_sensitivity.png") 
#         print("   - correlation_matrix.png")
#         print("   - af_vs_recall_correlation.png")
#         print("   - dataset_comparison.png (if multiple datasets)")
        
#     except FileNotFoundError:
#         print("❌ final_metrics_comparison.csv not found. Run parse_results.py first.")
#     except Exception as e:
#         print(f"❌ Error: {e}")
#         import traceback
#         traceback.print_exc()

# if __name__ == "__main__":
#     main()

# import pandas as pd
# import matplotlib.pyplot as plt
# import seaborn as sns
# import numpy as np
# import os

# # Styling για όμορφα διαγράμματα
# plt.style.use('default')
# sns.set_palette("husl")
# plt.rcParams['figure.figsize'] = (12, 8)
# plt.rcParams['font.size'] = 12

# def create_algorithm_comparison_plots(df):
#     """Δημιουργία διαγραμμάτων σύγκρισης αλγορίθμων"""
#     print("📊 Creating algorithm comparison plots...")
    
#     fig, axes = plt.subplots(2, 3, figsize=(20, 12))
#     fig.suptitle('Σύγκριση Αλγορίθμων Αναζήτησης - Τελικές Μετρικές', fontsize=16, fontweight='bold')
    
#     # 1. Bar plot - Average AF
#     af_data = df.groupby('Algorithm')['Avg_AF'].mean().sort_values()
#     axes[0,0].bar(af_data.index, af_data.values, color='skyblue', alpha=0.7)
#     axes[0,0].set_title('Μέσο Approximation Factor (AF)')
#     axes[0,0].set_ylabel('AF (πλησιέστερο στο 1 = καλύτερο)')
#     axes[0,0].tick_params(axis='x', rotation=45)
    
#     # Προσθήκη τιμών στις μπάρες
#     for i, v in enumerate(af_data.values):
#         axes[0,0].text(i, v + 0.01, f'{v:.3f}', ha='center', va='bottom')
    
#     # 2. Bar plot - Average Recall
#     recall_data = df.groupby('Algorithm')['Avg_Recall'].mean().sort_values(ascending=False)
#     axes[0,1].bar(recall_data.index, recall_data.values, color='lightgreen', alpha=0.7)
#     axes[0,1].set_title('Μέσος Recall@N')
#     axes[0,1].set_ylabel('Recall (υψηλότερο = καλύτερο)')
#     axes[0,1].set_ylim(0, 1)
#     axes[0,1].tick_params(axis='x', rotation=45)
    
#     for i, v in enumerate(recall_data.values):
#         axes[0,1].text(i, v + 0.01, f'{v:.3f}', ha='center', va='bottom')
    
#     # 3. Bar plot - QPS
#     qps_data = df.groupby('Algorithm')['QPS'].mean().sort_values(ascending=False)
#     axes[0,2].bar(qps_data.index, qps_data.values, color='salmon', alpha=0.7)
#     axes[0,2].set_title('Ταχύτητα (QPS)')
#     axes[0,2].set_ylabel('Queries Per Second (υψηλότερο = καλύτερο)')
#     axes[0,2].tick_params(axis='x', rotation=45)
    
#     for i, v in enumerate(qps_data.values):
#         axes[0,2].text(i, v + max(qps_data.values)*0.01, f'{v:.0f}', ha='center', va='bottom')
    
#     # 4. Scatter plot - AF vs Recall
#     colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         axes[1,0].scatter(algo_data['Avg_AF'], algo_data['Avg_Recall'], 
#                          label=algo, s=100, alpha=0.7, c=colors.get(algo, 'gray'))
    
#     axes[1,0].set_xlabel('Average AF')
#     axes[1,0].set_ylabel('Average Recall')
#     axes[1,0].set_title('AF vs Recall')
#     axes[1,0].legend()
#     axes[1,0].grid(True, alpha=0.3)
    
#     # 5. Scatter plot - Recall vs QPS
#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         axes[1,1].scatter(algo_data['Avg_Recall'], algo_data['QPS'], 
#                          label=algo, s=100, alpha=0.7, c=colors.get(algo, 'gray'))
    
#     axes[1,1].set_xlabel('Average Recall')
#     axes[1,1].set_ylabel('QPS')
#     axes[1,1].set_title('Recall vs Ταχύτητα')
#     axes[1,1].legend()
#     axes[1,1].grid(True, alpha=0.3)
    
#     # 6. BAR PLOT - Χρόνοι αναζήτησης (ΑΛΛΑΓΗ ΕΔΩ)
#     time_approx = df.groupby('Algorithm')['Avg_Approx_Time'].mean()
#     time_exact = df.groupby('Algorithm')['Avg_Exact_Time'].mean()
    
#     x = np.arange(len(time_approx.index))
#     width = 0.35
    
#     bars1 = axes[1,2].bar(x - width/2, time_approx.values, width, 
#                          label='Προσεγγιστική', color='lightcoral', alpha=0.7)
#     bars2 = axes[1,2].bar(x + width/2, time_exact.values, width, 
#                          label='Ακριβής', color='lightseagreen', alpha=0.7)
    
#     axes[1,2].set_xlabel('Αλγόριθμος')
#     axes[1,2].set_ylabel('Χρόνος (seconds)')
#     axes[1,2].set_title('Μέσοι Χρόνοι Αναζήτησης')
#     axes[1,2].set_xticks(x)
#     axes[1,2].set_xticklabels(time_approx.index, rotation=45)
#     axes[1,2].legend()
#     axes[1,2].grid(True, alpha=0.3, axis='y')
    
#     # Προσθήκη τιμών στις μπάρες
#     for bar in bars1:
#         height = bar.get_height()
#         axes[1,2].text(bar.get_x() + bar.get_width()/2., height + max(time_approx.max(), time_exact.max())*0.01,
#                       f'{height:.3f}s', ha='center', va='bottom', fontsize=9)
    
#     for bar in bars2:
#         height = bar.get_height()
#         axes[1,2].text(bar.get_x() + bar.get_width()/2., height + max(time_approx.max(), time_exact.max())*0.01,
#                       f'{height:.3f}s', ha='center', va='bottom', fontsize=9)
    
#     # Log scale αν οι χρόνοι ποικίλλουν πολύ
#     if time_approx.max() / time_approx.min() > 100 or time_exact.max() / time_exact.min() > 100:
#         axes[1,2].set_yscale('log')
    
#     plt.tight_layout()
#     plt.savefig('algorithm_comparison.png', dpi=300, bbox_inches='tight')
#     plt.show()
    
#     print("✅ Algorithm comparison plots saved!")

# def create_parameter_sensitivity_plots(df):
#     """Δημιουργία διαγραμμάτων ευαισθησίας παραμέτρων"""
#     print("📈 Creating parameter sensitivity plots...")
    
#     # LSH Parameter Analysis
#     lsh_data = df[df['Algorithm'] == 'LSH']
#     if not lsh_data.empty and 'k' in lsh_data.columns:
#         fig, axes = plt.subplots(2, 2, figsize=(15, 10))
#         fig.suptitle('Ευαισθησία Παραμέτρων LSH', fontsize=16, fontweight='bold')
        
#         # k vs AF & Recall
#         if 'k' in lsh_data.columns:
#             axes[0,0].plot(lsh_data['k'], lsh_data['Avg_AF'], 'o-', linewidth=2, markersize=8, label='AF')
#             axes[0,0].set_xlabel('k (Hash Functions)')
#             axes[0,0].set_ylabel('AF', color='blue')
#             axes[0,0].tick_params(axis='y', labelcolor='blue')
            
#             ax2 = axes[0,0].twinx()
#             ax2.plot(lsh_data['k'], lsh_data['Avg_Recall'], 's-', color='red', linewidth=2, markersize=6, label='Recall')
#             ax2.set_ylabel('Recall', color='red')
#             ax2.tick_params(axis='y', labelcolor='red')
#             ax2.set_ylim(0, 1)
            
#             axes[0,0].set_title('AF & Recall vs k')
#             axes[0,0].grid(True, alpha=0.3)
        
#         # L vs AF & Recall
#         if 'L' in lsh_data.columns:
#             axes[0,1].plot(lsh_data['L'], lsh_data['Avg_AF'], 'o-', linewidth=2, markersize=8, label='AF')
#             axes[0,1].set_xlabel('L (Hash Tables)')
#             axes[0,1].set_ylabel('AF', color='blue')
#             axes[0,1].tick_params(axis='y', labelcolor='blue')
            
#             ax2 = axes[0,1].twinx()
#             ax2.plot(lsh_data['L'], lsh_data['Avg_Recall'], 's-', color='red', linewidth=2, markersize=6, label='Recall')
#             ax2.set_ylabel('Recall', color='red')
#             ax2.tick_params(axis='y', labelcolor='red')
#             ax2.set_ylim(0, 1)
            
#             axes[0,1].set_title('AF & Recall vs L')
#             axes[0,1].grid(True, alpha=0.3)
        
#         # k vs QPS
#         if 'k' in lsh_data.columns:
#             axes[1,0].plot(lsh_data['k'], lsh_data['QPS'], 'o-', linewidth=2, markersize=8, color='green')
#             axes[1,0].set_xlabel('k (Hash Functions)')
#             axes[1,0].set_ylabel('QPS')
#             axes[1,0].set_title('Ταχύτητα (QPS) vs k')
#             axes[1,0].grid(True, alpha=0.3)
        
#         # L vs QPS
#         if 'L' in lsh_data.columns:
#             axes[1,1].plot(lsh_data['L'], lsh_data['QPS'], 'o-', linewidth=2, markersize=8, color='green')
#             axes[1,1].set_xlabel('L (Hash Tables)')
#             axes[1,1].set_ylabel('QPS')
#             axes[1,1].set_title('Ταχύτητα (QPS) vs L')
#             axes[1,1].grid(True, alpha=0.3)
        
#         plt.tight_layout()
#         plt.savefig('lsh_parameter_sensitivity.png', dpi=300, bbox_inches='tight')
#         plt.show()
#         print("✅ LSH parameter sensitivity plots saved!")

# def create_correlation_analysis(df):
#     """Ανάλυση συσχέτισης μεταξύ μετρικών"""
#     print("🔗 Creating correlation analysis...")
    
#     # Select numeric columns for correlation
#     numeric_cols = ['Avg_AF', 'Avg_Recall', 'QPS', 'Avg_Approx_Time', 'Avg_Exact_Time']
#     numeric_df = df[numeric_cols].select_dtypes(include=[np.number])
    
#     # Correlation matrix
#     corr_matrix = numeric_df.corr()
    
#     plt.figure(figsize=(10, 8))
#     mask = np.triu(np.ones_like(corr_matrix, dtype=bool))  # Μask για πάνω τρίγωνο
#     sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0,
#                 square=True, fmt='.2f', cbar_kws={'shrink': 0.8}, mask=mask)
#     plt.title('Μητρώο Συσχέτισης Μετρικών Απόδοσης', fontsize=16, fontweight='bold')
#     plt.tight_layout()
#     plt.savefig('correlation_matrix.png', dpi=300, bbox_inches='tight')
#     plt.show()
    
#     # AF vs Recall scatter plot με regression line
#     plt.figure(figsize=(10, 6))
#     for algo in df['Algorithm'].unique():
#         algo_data = df[df['Algorithm'] == algo]
#         plt.scatter(algo_data['Avg_AF'], algo_data['Avg_Recall'], 
#                    label=algo, s=80, alpha=0.7)
    
#     # Προσθήκη regression line
#     z = np.polyfit(df['Avg_AF'], df['Avg_Recall'], 1)
#     p = np.poly1d(z)
#     plt.plot(df['Avg_AF'], p(df['Avg_AF']), "r--", alpha=0.8, label='Regression Line')
    
#     plt.xlabel('Average Approximation Factor (AF)')
#     plt.ylabel('Average Recall@N')
#     plt.title('Συσχέτιση AF και Recall')
#     plt.legend()
#     plt.grid(True, alpha=0.3)
#     plt.savefig('af_vs_recall_correlation.png', dpi=300, bbox_inches='tight')
#     plt.show()
    
#     print("✅ Correlation analysis plots saved!")

# def create_dataset_comparison(df):
#     """Σύγκριση απόδοσης μεταξύ datasets"""
#     if 'Dataset' in df.columns and df['Dataset'].nunique() > 1:
#         print("📁 Creating dataset comparison plots...")
        
#         dataset_data = df.groupby(['Algorithm', 'Dataset']).agg({
#             'Avg_AF': 'mean',
#             'Avg_Recall': 'mean',
#             'QPS': 'mean'
#         }).reset_index()
        
#         fig, axes = plt.subplots(1, 3, figsize=(18, 6))
#         fig.suptitle('Σύγκριση Απόδοσης Ανά Dataset', fontsize=16, fontweight='bold')
        
#         # AF ανά dataset
#         sns.barplot(data=dataset_data, x='Algorithm', y='Avg_AF', hue='Dataset', ax=axes[0])
#         axes[0].set_title('Μέσο AF Ανά Dataset')
#         axes[0].set_ylabel('AF')
#         axes[0].tick_params(axis='x', rotation=45)
        
#         # Recall ανά dataset
#         sns.barplot(data=dataset_data, x='Algorithm', y='Avg_Recall', hue='Dataset', ax=axes[1])
#         axes[1].set_title('Μέσο Recall Ανά Dataset')
#         axes[1].set_ylabel('Recall')
#         axes[1].tick_params(axis='x', rotation=45)
        
#         # QPS ανά dataset
#         sns.barplot(data=dataset_data, x='Algorithm', y='QPS', hue='Dataset', ax=axes[2])
#         axes[2].set_title('Ταχύτητα (QPS) Ανά Dataset')
#         axes[2].set_ylabel('QPS')
#         axes[2].tick_params(axis='x', rotation=45)
        
#         plt.tight_layout()
#         plt.savefig('dataset_comparison.png', dpi=300, bbox_inches='tight')
#         plt.show()
#         print("✅ Dataset comparison plots saved!")

# def main():
#     """Κύρια συνάρτηση δημιουργίας διαγραμμάτων"""
#     try:
#         # Διάβασε τα αποτελέσματα από το νέο parser
#         df = pd.read_csv('final_metrics_comparison.csv')
#         print(f"📊 Loaded {len(df)} experiment results from final_metrics_comparison.csv")
        
#         # Δημιουργία όλων των διαγραμμάτων
#         create_algorithm_comparison_plots(df)
#         create_parameter_sensitivity_plots(df)
#         create_correlation_analysis(df)
#         create_dataset_comparison(df)
        
#         print("\n🎉 All plots generated successfully!")
#         print("📁 Generated files:")
#         print("   - algorithm_comparison.png")
#         print("   - lsh_parameter_sensitivity.png") 
#         print("   - correlation_matrix.png")
#         print("   - af_vs_recall_correlation.png")
#         print("   - dataset_comparison.png (if multiple datasets)")
        
#     except FileNotFoundError:
#         print("❌ final_metrics_comparison.csv not found. Run parse_results.py first.")
#     except Exception as e:
#         print(f"❌ Error: {e}")
#         import traceback
#         traceback.print_exc()

# if __name__ == "__main__":
#     main()


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

def create_comprehensive_comparison_plots(df):
    """Δημιουργία διαγραμμάτων συγκριτικής ανάλυσης μεταξύ μετρικών"""
    print("📈 Creating comprehensive metric comparison plots...")
    
    # 1. RADAR CHART - Σύγκριση όλων των μετρικών για κάθε αλγόριθμο
    create_radar_chart(df)
    
    # 2. HEATMAP - Σύγκριση απόδοσης ανά αλγόριθμο και μετρική
    create_performance_heatmap(df)
    
    # 3. PARALLEL COORDINATES - Πολυμεταβλητή ανάλυση
    create_parallel_coordinates(df)
    
    # 4. PAIRPLOT - Όλες οι συσχετίσεις μεταξύ μετρικών
    create_metric_pairplot(df)

def create_radar_chart(df):
    """Radar chart για σύγκριση όλων των μετρικών"""
    # Κανονικοποίηση των μετρικών για radar chart (0-1)
    metrics_to_compare = ['Avg_AF', 'Avg_Recall', 'QPS', 'Avg_Approx_Time']
    
    # Υπολογισμός μέσων όρων ανά αλγόριθμο
    algo_means = df.groupby('Algorithm')[metrics_to_compare].mean()
    
    # Κανονικοποίηση (για AF θέλουμε τιμές κοντά στο 1 να είναι καλύτερες)
    normalized = algo_means.copy()
    normalized['Avg_AF'] = 1 / normalized['Avg_AF']  # Αντιστροφή για AF
    normalized['Avg_Approx_Time'] = 1 / normalized['Avg_Approx_Time']  # Αντιστροφή για χρόνο
    
    # Κανονικοποίηση 0-1
    for col in normalized.columns:
        normalized[col] = (normalized[col] - normalized[col].min()) / (normalized[col].max() - normalized[col].min())
    
    # Ρυθμίσεις για radar chart
    categories = ['Accuracy (AF)', 'Recall', 'Speed (QPS)', 'Efficiency (Time)']
    N = len(categories)
    
    # Γωνίες για κάθε κατηγορία
    angles = [n / float(N) * 2 * np.pi for n in range(N)]
    angles += angles[:1]  # Κλείσιμο του chart
    
    fig, ax = plt.subplots(figsize=(10, 10), subplot_kw=dict(projection='polar'))
    
    # Χρώματα για κάθε αλγόριθμο
    colors = ['red', 'blue', 'green', 'orange', 'purple']
    
    for idx, (algo, values) in enumerate(normalized.iterrows()):
        values_plot = values.tolist()
        values_plot += values_plot[:1]  # Κλείσιμο
        
        ax.plot(angles, values_plot, 'o-', linewidth=2, label=algo, color=colors[idx % len(colors)])
        ax.fill(angles, values_plot, alpha=0.1, color=colors[idx % len(colors)])
    
    ax.set_xticks(angles[:-1])
    ax.set_xticklabels(categories)
    ax.set_ylim(0, 1)
    ax.set_title('Συνολική Απόδοση Αλγορίθμων - Radar Chart\n(Υψηλότερες τιμές = Καλύτερες)', 
                 size=14, fontweight='bold', pad=20)
    ax.legend(loc='upper right', bbox_to_anchor=(1.3, 1.0))
    
    plt.tight_layout()
    plt.savefig('radar_chart_comparison.png', dpi=300, bbox_inches='tight')
    plt.show()
    print("✅ Radar chart comparison saved!")

def create_performance_heatmap(df):
    """Heatmap για οπτικοποίηση απόδοσης ανά αλγόριθμο και μετρική"""
    # Επιλογή και προετοιμασία μετρικών
    performance_metrics = ['Avg_AF', 'Avg_Recall', 'QPS', 'Avg_Approx_Time']
    
    # Υπολογισμός μέσων όρων
    algo_performance = df.groupby('Algorithm')[performance_metrics].mean()
    
    # Κανονικοποίηση για καλύτερη οπτικοποίηση (0-1)
    normalized_perf = algo_performance.copy()
    normalized_perf['Avg_AF'] = 1 / normalized_perf['Avg_AF']  # Αντιστροφή για AF
    normalized_perf['Avg_Approx_Time'] = 1 / normalized_perf['Avg_Approx_Time']  # Αντιστροφή για χρόνο
    
    for col in normalized_perf.columns:
        normalized_perf[col] = (normalized_perf[col] - normalized_perf[col].min()) / \
                              (normalized_perf[col].max() - normalized_perf[col].min())
    
    # Δημιουργία heatmap
    plt.figure(figsize=(12, 8))
    sns.heatmap(normalized_perf, 
                annot=algo_performance.round(3),  # Εμφάνιση πραγματικών τιμών
                fmt='.3f',
                cmap='YlOrRd',
                cbar_kws={'label': 'Κανονικοποιημένη Απόδοση (0-1)'},
                square=True)
    
    plt.title('Σύγκριση Απόδοσης Αλγορίθμων - Heatmap\n(Σκούρα χρώματα = Καλύτερη απόδοση)', 
              fontsize=16, fontweight='bold', pad=20)
    plt.xlabel('Μετρικές Απόδοσης')
    plt.ylabel('Αλγόριθμοι')
    plt.xticks(rotation=45)
    
    plt.tight_layout()
    plt.savefig('performance_heatmap.png', dpi=300, bbox_inches='tight')
    plt.show()
    print("✅ Performance heatmap saved!")

def create_parallel_coordinates(df):
    """Parallel coordinates plot για πολυμεταβλητή ανάλυση"""
    # Επιλογή μετρικών για σύγκριση
    metrics = ['Avg_AF', 'Avg_Recall', 'QPS', 'Avg_Approx_Time']
    
    # Κανονικοποίηση
    plot_data = df[['Algorithm'] + metrics].copy()
    
    for metric in metrics:
        if metric in ['Avg_AF', 'Avg_Approx_Time']:
            # Αντιστροφή για μετρικές όπου μικρότερες τιμές = καλύτερες
            plot_data[metric] = 1 / plot_data[metric]
        plot_data[metric] = (plot_data[metric] - plot_data[metric].min()) / \
                           (plot_data[metric].max() - plot_data[metric].min())
    
    # Δημιουργία parallel coordinates
    plt.figure(figsize=(15, 8))
    parallel_coord = pd.plotting.parallel_coordinates(plot_data, 'Algorithm', 
                                                     color=['red', 'blue', 'green', 'orange'])
    
    plt.title('Πολυμεταβλητή Σύγκριση Αλγορίθμων - Parallel Coordinates', 
              fontsize=16, fontweight='bold', pad=20)
    plt.ylabel('Κανονικοποιημένη Απόδοση (0-1)')
    plt.grid(True, alpha=0.3)
    
    # Προσαρμογή labels
    new_labels = ['Accuracy (AF)', 'Recall', 'Speed (QPS)', 'Efficiency (Time)']
    if len(plt.gca().get_xticklabels()) == len(new_labels):
        for i, label in enumerate(plt.gca().get_xticklabels()):
            label.set_text(new_labels[i])
        plt.gca().set_xticklabels(new_labels)
    
    plt.tight_layout()
    plt.savefig('parallel_coordinates.png', dpi=300, bbox_inches='tight')
    plt.show()
    print("✅ Parallel coordinates plot saved!")

def create_metric_pairplot(df):
    """Pairplot για όλες τις συσχετίσεις μεταξύ μετρικών"""
    # Επιλογή αριθμητικών στηλών
    numeric_metrics = ['Avg_AF', 'Avg_Recall', 'QPS', 'Avg_Approx_Time', 'Avg_Exact_Time']
    plot_data = df[['Algorithm'] + numeric_metrics].copy()
    
    # Δημιουργία pairplot
    plt.figure(figsize=(15, 12))
    pairplot = sns.pairplot(plot_data, 
                           hue='Algorithm',
                           diag_kind='hist',
                           palette={'LSH': 'red', 'Hypercube': 'blue', 
                                  'IVFFlat': 'green', 'IVFPQ': 'orange'},
                           plot_kws={'alpha': 0.7, 's': 80},
                           diag_kws={'alpha': 0.7})
    
    pairplot.fig.suptitle('Συσχετίσεις Μεταξύ Όλων των Μετρικών - Pairplot', 
                         fontsize=16, fontweight='bold', y=1.02)
    
    plt.tight_layout()
    plt.savefig('metric_correlations_pairplot.png', dpi=300, bbox_inches='tight')
    plt.show()
    print("✅ Metric correlations pairplot saved!")

def create_ranking_analysis(df):
    """Ανάλυση κατάταξης αλγορίθμων ανά μετρική"""
    metrics = ['Avg_AF', 'Avg_Recall', 'QPS']
    
    # Υπολογισμός μέσων όρων
    algo_means = df.groupby('Algorithm')[metrics].mean()
    
    # Κατάταξη (1 = καλύτερος)
    rankings = algo_means.rank(ascending=False)  # Για Recall και QPS
    rankings['Avg_AF'] = algo_means['Avg_AF'].rank(ascending=True)  # Για AF (μικρότερο = καλύτερο)
    
    # Δημιουργία bar plot για κατατάξεις
    fig, axes = plt.subplots(1, 3, figsize=(18, 6))
    fig.suptitle('Κατάταξη Αλγορίθμων Ανά Μετρική (1 = Καλύτερος)', fontsize=16, fontweight='bold')
    
    for i, metric in enumerate(metrics):
        sorted_ranks = rankings[metric].sort_values()
        colors = ['gold' if rank == 1 else 'silver' if rank == 2 else 'lightblue' for rank in sorted_ranks]
        
        bars = axes[i].bar(sorted_ranks.index, sorted_ranks.values, color=colors, alpha=0.7)
        axes[i].set_title(f'Κατάταξη - {metric}')
        axes[i].set_ylabel('Θέση Κατάταξης')
        axes[i].tick_params(axis='x', rotation=45)
        
        # Προσθήκη τιμών
        for bar, rank in zip(bars, sorted_ranks.values):
            axes[i].text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.1,
                        f'{int(rank)}η', ha='center', va='bottom', fontweight='bold')
    
    plt.tight_layout()
    plt.savefig('algorithm_rankings.png', dpi=300, bbox_inches='tight')
    plt.show()
    print("✅ Algorithm rankings analysis saved!")

# ΑΝΑΝΕΩΜΕΝΗ MAIN FUNCTION
def main():
    """Κύρια συνάρτηση δημιουργίας διαγραμμάτων"""
    try:
        # Διάβασε τα αποτελέσματα
        df = pd.read_csv('final_metrics_comparison.csv')
        print(f"📊 Loaded {len(df)} experiment results from final_metrics_comparison.csv")
        
        # Δημιουργία ΟΛΩΝ των διαγραμμάτων
        create_algorithm_comparison_plots(df)  # Πρωτότυπα plots
        create_parameter_sensitivity_plots(df)
        create_correlation_analysis(df)
        create_dataset_comparison(df)
        
        # ΝΕΑ: Συγκριτική ανάλυση μεταξύ μετρικών
        create_comprehensive_comparison_plots(df)
        create_ranking_analysis(df)
        
        print("\n🎉 ALL PLOTS GENERATED SUCCESSFULLY!")
        print("📁 GENERATED FILES:")
        print("   - algorithm_comparison.png")
        print("   - lsh_parameter_sensitivity.png") 
        print("   - correlation_matrix.png")
        print("   - af_vs_recall_correlation.png")
        print("   - dataset_comparison.png")
        print("   - radar_chart_comparison.png")
        print("   - performance_heatmap.png")
        print("   - parallel_coordinates.png")
        print("   - metric_correlations_pairplot.png")
        print("   - algorithm_rankings.png")
        
    except FileNotFoundError:
        print("❌ final_metrics_comparison.csv not found. Run parse_results.py first.")
    except Exception as e:
        print(f"❌ Error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()