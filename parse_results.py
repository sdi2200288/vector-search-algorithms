# SCRIPT ΕΞΑΓΩΓΗΣ ΜΕΤΡΙΚΩΝ ΑΠΟ OUTPUT FILES

import os
import pandas as pd
import re
import glob

def parse_final_metrics(output_file):
    """Εξάγει μόνο τις ΤΕΛΙΚΕΣ ΜΕΤΡΙΚΕΣ για κάθε εκτέλεση"""
    try:
        with open(output_file, 'r', encoding='utf-8') as file:
            content = file.read()

        # Εύρεση αλγόριθμου
        algorithm_match = re.search(r'(LSH|Hypercube|IVFFlat|IVFPQ)', content)
        algorithm = algorithm_match.group(1) if algorithm_match else "Unknown"

        # Εξαγωγή FINAL METRICS
        final_metrics = {
            'Algorithm': algorithm,
            'File': os.path.basename(output_file),
            'Dataset': 'MNIST' if 'mnist' in output_file.lower() else 'SIFT',
            'Avg_AF': extract_final_metric(r'Total Average AF: (\d+\.\d+)', content),
            'Avg_Recall': extract_final_metric(r'Total Average Recall@N: (\d+\.\d+)', content),
            'QPS': extract_final_metric(r'QPS: (\d+\.\d+)', content),
            'Avg_Approx_Time': extract_final_metric(r'Average Approximate Time: (\d+\.\d+)', content),
            'Avg_Exact_Time': extract_final_metric(r'Average Exact Time: (\d+\.\d+)', content),
        }
        
        # Εξαγωγή παραμέτρων
        final_metrics.update(extract_params(output_file))
        
        print(f"✅ Extracted final metrics from {output_file}")
        return final_metrics

    except Exception as ex:
        print(f"❌ Error parsing file {output_file}: {ex}")
        return None

def extract_final_metric(pattern, content):
    """Εξάγει την τιμή από το pattern για FINAL METRICS"""
    match = re.search(pattern, content)
    return float(match.group(1)) if match else 0.0

def extract_params(output):
    """Εξάγει παραμέτρους από το όνομα του αρχείου"""
    params = {}
    output_lower = output.lower()

    # LSH params
    if 'lsh' in output_lower:
        k_match = re.search(r'k(\d+)', output_lower)
        L_match = re.search(r'l(\d+)', output_lower)
        w_match = re.search(r'w([\d\.]+)', output_lower)
        params.update({
            'k': int(k_match.group(1)) if k_match else 4,
            'L': int(L_match.group(1)) if L_match else 5,
            'w': float(w_match.group(1)) if w_match else 4.0
        })

    # Hypercube params
    elif 'hypercube' in output_lower or 'hyp' in output_lower:
        kproj_match = re.search(r'kproj(\d+)', output_lower)
        m_match = re.search(r'm(\d+)', output_lower)
        probes_match = re.search(r'probes?(\d+)', output_lower)
        w_match = re.search(r'w([\d\.]+)', output_lower)
        params.update({
            'kproj': int(kproj_match.group(1)) if kproj_match else 14,
            'M': int(m_match.group(1)) if m_match else 10,
            'probes': int(probes_match.group(1)) if probes_match else 2,
            'w': float(w_match.group(1)) if w_match else 4.0
        })

    # IVFFlat params
    elif 'ivfflat' in output_lower or 'flat' in output_lower:
        kcluster_match = re.search(r'k(\d+)', output_lower) or re.search(r'kcluster(\d+)', output_lower)
        nprobe_match = re.search(r'nprobe?(\d+)', output_lower)
        params.update({
            'kclusters': int(kcluster_match.group(1)) if kcluster_match else 50,
            'nprobe': int(nprobe_match.group(1)) if nprobe_match else 5
        })

    # IVFPQ params
    elif 'ivfpq' in output_lower or 'pq' in output_lower:
        kcluster_match = re.search(r'k(\d+)', output_lower) or re.search(r'kcluster(\d+)', output_lower)
        nprobe_match = re.search(r'nprobe?(\d+)', output_lower)
        m_match = re.search(r'm(\d+)', output_lower)
        nbits_match = re.search(r'nbits?(\d+)', output_lower)
        params.update({
            'kclusters': int(kcluster_match.group(1)) if kcluster_match else 50,
            'nprobe': int(nprobe_match.group(1)) if nprobe_match else 5,
            'M': int(m_match.group(1)) if m_match else 16,
            'nbits': int(nbits_match.group(1)) if nbits_match else 8
        })

    return params

def create_comparison_table(df):
    """Δημιουργεί πίνακα σύγκρισης για όλους τους αλγορίθμους"""
    comparison_data = []
    
    for _, row in df.iterrows():
        # Δημιουργία περιγραφής παραμέτρων ανά αλγόριθμο
        if row['Algorithm'] == 'LSH':
            params_desc = f"k={row.get('k', 4)}, L={row.get('L', 5)}, w={row.get('w', 4.0)}"
        elif row['Algorithm'] == 'Hypercube':
            params_desc = f"kproj={row.get('kproj', 14)}, M={row.get('M', 10)}, probes={row.get('probes', 2)}"
        elif row['Algorithm'] == 'IVFFlat':
            params_desc = f"kclusters={row.get('kclusters', 50)}, nprobe={row.get('nprobe', 5)}"
        elif row['Algorithm'] == 'IVFPQ':
            params_desc = f"kclusters={row.get('kclusters', 50)}, nprobe={row.get('nprobe', 5)}, M={row.get('M', 16)}, nbits={row.get('nbits', 8)}"
        else:
            params_desc = "Default"
        
        comparison_data.append({
            'Algorithm': row['Algorithm'],
            'Parameters': params_desc,
            'Dataset': row['Dataset'],
            'Avg AF': row['Avg_AF'],
            'Recall@N': row['Avg_Recall'],
            'QPS': row['QPS'],
            'Avg Approx Time (s)': row['Avg_Approx_Time'],
            'Avg Exact Time (s)': row['Avg_Exact_Time'],
            'File': row['File']
        })
    
    return pd.DataFrame(comparison_data)

def main():
    """Βασική συνάρτηση επεξεργασίας αποτελεσμάτων"""
    
    # Βρίσκει όλα τα output files
    output_files = glob.glob("*.dat") + glob.glob("*.txt")  # Προσθήκη .txt αν χρειαστεί
    all_final_metrics = []
    
    for file in output_files:
        print(f"📖 Processing {file}...")
        final_metrics = parse_final_metrics(file)
        if final_metrics:
            all_final_metrics.append(final_metrics)
    
    if not all_final_metrics:
        print("❌ No results found!")
        return None
    
    # Create dataframe
    df = pd.DataFrame(all_final_metrics)

    # Save final metrics to CSV
    df.to_csv('final_metrics_comparison.csv', index=False)
    print(f"✅ Saved {len(df)} final metrics to final_metrics_comparison.csv")

    # Create and display comparison table
    comparison_df = create_comparison_table(df)
    
    print("\n" + "="*80)
    print("📊 FINAL METRICS COMPARISON TABLE")
    print("="*80)
    
    # Εμφάνιση πίνακα σύγκρισης
    pd.set_option('display.max_columns', None)
    pd.set_option('display.width', 1000)
    
    print(comparison_df.to_string(index=False))
    
    # Αποθήκευση πίνακα σύγκρισης
    comparison_df.to_csv('algorithm_comparison_table.csv', index=False)
    print(f"\n✅ Saved comparison table to algorithm_comparison_table.csv")
    
    return comparison_df

if __name__ == "__main__":
    df = main()


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
    plt.show()

    # 2. BAR CHART: QPS ανά αλγόριθμο
    plt.figure(figsize=(10, 6))
    qps_by_algorithm = df.groupby('Algorithm')['QPS'].mean().sort_values(ascending=False)
    qps_by_algorithm.plot(kind='bar', color='lightgreen')
    plt.title('Average QPS by Algorithm')
    plt.ylabel('Queries Per Second')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('qps_by_algorithm.png', dpi=300, bbox_inches='tight')
    plt.show()

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
    plt.show()

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
    plt.show()

def analyze_parameter_effects(df):
    """Αναλύει την επίδραση των παραμέτρων"""
    
    # Για LSH: Επίδραση του L στο Recall
    lsh_data = df[df['Algorithm'] == 'LSH']
    if not lsh_data.empty:
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
        plt.show()

    # Για IVFFlat: Επίδραση του nprobe
    ivf_data = df[df['Algorithm'] == 'IVFFlat']
    if not ivf_data.empty:
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
        plt.show()

def generate_summary_statistics(df):
    """Δημιουργεί σύνοψη στατιστικών"""
    print("="*60)
    print("📈 SUMMARY STATISTICS")
    print("="*60)
    
    # Καλύτερος αλγόριθμος ανά μετρική
    best_recall = df.loc[df['Recall@N'].idxmax()]
    best_qps = df.loc[df['QPS'].idxmax()]
    best_af = df.loc[df['Avg AF'].idxmin()]
    
    print(f"🎯 Best Recall@N: {best_recall['Recall@N']:.3f} ({best_recall['Algorithm']} - {best_recall['Parameters']})")
    print(f"⚡ Best QPS: {best_qps['QPS']:.1f} ({best_qps['Algorithm']} - {best_qps['Parameters']})")
    print(f"📏 Best AF (closest to 1): {best_af['Avg AF']:.3f} ({best_af['Algorithm']} - {best_af['Parameters']})")
    
    print("\n📊 Average by Algorithm:")
    avg_by_algo = df.groupby('Algorithm')[['Recall@N', 'QPS', 'Avg AF']].mean()
    print(avg_by_algo.round(3))

# Κύριο πρόγραμμα ανάλυσης
def main_analysis():
    # Φόρτωση των δεδομένων
    df = pd.read_csv('algorithm_comparison_table.csv')
    
    # Δημιουργία γραφημάτων
    create_comparison_plots(df)
    analyze_parameter_effects(df)
    generate_summary_statistics(df)
    
    # Αποθήκευση αναλυτικού πίνακα
    summary_table = df.groupby(['Algorithm', 'Dataset']).agg({
        'Recall@N': 'mean',
        'QPS': 'mean', 
        'Avg AF': 'mean',
        'Avg Approx Time (s)': 'mean'
    }).round(3)
    
    summary_table.to_csv('detailed_summary.csv')
    print(f"\n✅ Saved detailed summary to detailed_summary.csv")

if __name__ == "__main__":
    main_analysis()