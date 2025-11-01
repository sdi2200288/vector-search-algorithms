#SCRIPT ΔΗΜΙΟΥΡΓΙΑΣ ΔΙΑΓΡΑΜΜΑΤΩΝ

import matplotlib
matplotlib.use('Agg')           #απενεργοποιεί το GUI backend για να μην εμφανίζονται warnings
import matplotlib.pyplot as plt #βιβλιοθήκη για δημιουργία γραφημάτων 
import seaborn as sns           #βιβλιοθήκη για βελτιωμένων γραφημάτων 
import pandas as pd             #βιβλιοθήκη για χειρισμό δεδομένων
import numpy as np              #βιβλιοθήκη για αριθμητικούς υπολογισμούς 


#συνάρτηση που δημιουργεί ξεχωριστά scatter plots για κάθε ζεύγος μετρικών
def create_scatter_pairwise_comparisons(df):        
    #λίστα με ζεύγη μετρικών που θα συγκριθούν
    metrics_pairs = [       
        ('Recall@N', 'QPS'),
        ('Recall@N', 'Avg AF'),
        ('Recall@N', 'Avg Approx Time (s)'),
        ('QPS', 'Avg AF'),
        ('QPS', 'Avg Approx Time (s)'),
        ('Avg AF', 'Avg Approx Time (s)'),
        ('Avg Approx Time (s)', 'Avg Exact Time (s)')
    ]
    
    #χρώματα για κάθε αλγόριθμο
    colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
    
    for metric_x, metric_y in metrics_pairs:                    #επανάληψη για κάθε ζεύγος μετρικών
        if metric_x in df.columns and metric_y in df.columns:   #έλεγχος ύπαρξης στηλών στο dataframe
            plt.figure(figsize=(10, 6))                         #δημιουργία νέου σχήματος γραφήματος
            
            for algo in df['Algorithm'].unique():                       #για κάθε αλγόριθμο
                algo_data = df[df['Algorithm'] == algo]                 #φιλτράρουμε τα δεδομένα για τον τρέχοντα αλγόριθμο
                plt.scatter(algo_data[metric_x], algo_data[metric_y],   #δημιουργία scatter plot
                           c=colors.get(algo, 'gray'), label=algo, alpha=0.7, s=100)
            
            plt.xlabel(metric_x)                        #ορισμός ετικετών αξόνων
            plt.ylabel(metric_y)
            plt.title(f'{metric_y} vs {metric_x}')      #τίτλος γραφήματος
            plt.legend()                                #εμφάνιση λεζάντας
            plt.grid(True, alpha=0.3)                   #προσθήκη πλέγματος
            
            #προσθήκη γραμμής τάσης
            try:
                #υπολογισμός γραμμικής παλινδρόμησης 
                z = np.polyfit(df[metric_x], df[metric_y], 1)
                p = np.poly1d(z)

                #σχεδίαση γραμμής τάσης
                plt.plot(df[metric_x], p(df[metric_x]), "r--", alpha=0.8, 
                        label=f'Trend (ρ={df[metric_x].corr(df[metric_y]):.2f})')
                plt.legend()
            except:
                pass    #για την αντιμετώπιση σφάλματος στον υπολογισμό τάσης
            
            
            filename = f'comparison_{metric_x.replace(" ", "_").replace("(", "").replace(")", "")}_vs_{metric_y.replace(" ", "_").replace("(", "").replace(")", "")}.png'   #δημιουργία ονόματος αρχείου 
            plt.tight_layout()                                      #βελτιστοποίηση της διάταξης
            plt.savefig(filename, dpi=300, bbox_inches='tight')     #αποθήκευση του γραφήματος 
            plt.close()                                             #κλείσιμο γραφήματος για εξοικονόμηση μνήμης 
            print(f"Saved {filename}")                              #εκτύπωση μηνύματος επιβεβαίωσης 


#συνάρτηση που δημιουργεί ξεχωριστά bar charts για κάθε ζεύγος μετρικών - ΧΩΡΙΣ error bars
def create_bar_pairwise_comparisons(df):    
    #λίστα με ζεύγη μετρικών για σύγκριση
    metrics_pairs = [
        ('Recall@N', 'QPS'),
        ('Recall@N', 'Avg AF'),
        ('Recall@N', 'Avg Approx Time (s)'),
        ('QPS', 'Avg AF'),
        ('QPS', 'Avg Approx Time (s)'),
        ('Avg AF', 'Avg Approx Time (s)'),
        ('Avg Approx Time (s)', 'Avg Exact Time (s)')
    ]
    
    #χρώματα για κάθε αλγόριθμο
    colors = {'LSH': 'red', 'Hypercube': 'blue', 'IVFFlat': 'green', 'IVFPQ': 'orange'}
    
    for metric_x, metric_y in metrics_pairs:                    #επανάηληψη για κάθε ζεύγος μετρικών
        if metric_x in df.columns and metric_y in df.columns:   #έλεγχος ύπαρξης στηλών 
            fig, ax = plt.subplots(figsize=(12, 8))             #δημιουργία υπογραφήματος
            
            algorithms = df['Algorithm'].unique()               #λίστα μοναδικών αλγορίθμων
            x_pos = np.arange(len(algorithms))                  #θέσεις στον άξονα x για τις μπάρες
            width = 0.35                                        #πλάτος κάθε μπάρας
            
            #λίστες για μέσους όρους 
            means_x = []
            means_y = []
            
            for algo in algorithms:                         #υπολογισμός μέσων όρων για κάθε αλγόριθμο
                algo_data = df[df['Algorithm'] == algo]     #φιλτράρισμα δεδομένων του αλγορίθμου
                means_x.append(algo_data[metric_x].mean())  #υπολογισμός μέσου όρου της πρώτης μετρικής
                means_y.append(algo_data[metric_y].mean())  #υπολογισμός μέσου όρου δεύτερης μετρικής
            
            #δημιουργία πρώτης ομάδας μπαρών 
            bars1 = ax.bar(x_pos - width/2, means_x, width, 
                          label=metric_x, alpha=0.8, 
                          color='skyblue')  #χωρίς error bars
            
            #δημιουργία δεύτερης ομάδας μπαρών
            bars2 = ax.bar(x_pos + width/2, means_y, width, 
                          label=metric_y, alpha=0.8, 
                          color='lightcoral')  #χωρίς error bars
            
            ax.set_xlabel('Algorithms')                                             #ορισμός ετικετών των αξόνων
            ax.set_ylabel('Metric Values')
            ax.set_title(f'Comparison: {metric_x} vs {metric_y} by Algorithm')      #τίτλος γραφήματος
            ax.set_xticks(x_pos)                                                    #ορισμός θέσεων και ετικετών στον άξονα x
            ax.set_xticklabels(algorithms, rotation=45)
            ax.legend()                                                             #εμφάνιση λεζάντας
            
            #απενεργοποίηση πλέγματος και πλαισίου
            ax.grid(False)
            ax.spines['top'].set_visible(False)
            ax.spines['right'].set_visible(False)
            
            for bars in [bars1, bars2]:         #προσθήκη τιμών πάνω από τις ράβδους
                for bar in bars:
                    height = bar.get_height()   #λήψη του ύψους της μπάρας 
                    ax.text(bar.get_x() + bar.get_width()/2., height + 0.01 * max(means_x + means_y),       #προσθήκη κειμένου με τηην τιμή 
                           f'{height:.3f}', ha='center', va='bottom', fontsize=8)
            
            plt.tight_layout()           #βελτιστοποίηση διάταξης
            filename = f'bar_comparison_{metric_x.replace(" ", "_").replace("(", "").replace(")", "")}_vs_{metric_y.replace(" ", "_").replace("(", "").replace(")", "")}.png'   #δημιουργία ονόματος αρχείου
            plt.savefig(filename, dpi=300, bbox_inches='tight', 
                       pad_inches=0.1, facecolor='white', edgecolor='none')     #αποθήκευση γραφήματος 
            plt.close()                  #κλείσιμο γραφήματος 
            print(f"Saved {filename}")   #εκτύπωση μηνύματος επιβεβαίωσης 


#συνάρτηση που δημιουργεί γραφήματα σύγκρισης
def create_comparison_plots(df):
    #1. BAR CHART: Recall@N ανά αλγόριθμο
    plt.figure(figsize=(10, 6))

    #ομαδοποίηση και υπολογισμός μέσου όρου Recall@N
    recall_by_algorithm = df.groupby('Algorithm')['Recall@N'].mean().sort_values(ascending=False)   
    
    #δημιουργία του bar chart
    recall_by_algorithm.plot(kind='bar', color='skyblue')
    plt.title('Average Recall@N by Algorithm')
    plt.ylabel('Recall@N')
    plt.xticks(rotation=45)
    plt.tight_layout()

    #αποθήκευση γραφήματος
    plt.savefig('recall_by_algorithm.png', dpi=300, bbox_inches='tight')
    plt.close()

    #2. BAR CHART: QPS ανά αλγόριθμο
    plt.figure(figsize=(10, 6))
    
    #ομαδοποίηση και υπολογισμός μέσου όρου QPS
    qps_by_algorithm = df.groupby('Algorithm')['QPS'].mean().sort_values(ascending=False)

    #δημιουργία του bar chart
    qps_by_algorithm.plot(kind='bar', color='lightgreen')
    plt.title('Average QPS by Algorithm')
    plt.ylabel('Queries Per Second')
    plt.xticks(rotation=45)
    plt.tight_layout()

    #αποθήκευση γραφήματος
    plt.savefig('qps_by_algorithm.png', dpi=300, bbox_inches='tight')
    plt.close()

    #3. BAR CHART: AF ανά αλγόριθμο
    plt.figure(figsize=(10, 6))

    #ομαδοποίηση και υπολογισμός μέσου όρου AF
    af_by_algorithm = df.groupby('Algorithm')['Avg AF'].mean().sort_values(ascending=False)
    
    #δημιουργία του bar chart
    af_by_algorithm.plot(kind='bar', color='lightcoral')
    plt.title('Average AF by Algorithm')
    plt.ylabel('Avg AF')
    plt.xticks(rotation=45)
    plt.tight_layout()

    #αποθήκευση γραφήματος
    plt.savefig('af_by_algorithm.png', dpi=300, bbox_inches='tight')
    plt.close()

    #4. BAR CHART: Avg Approx Time (s) ανά αλγόριθμο
    plt.figure(figsize=(10, 6))
    
    #ομαδοποίηση και υπολογισμός μέσου χρόνου προσέγγισης
    approx_time_by_algorithm = df.groupby('Algorithm')['Avg Approx Time (s)'].mean().sort_values(ascending=False)
   
    #δημιουργία του bar chart
    approx_time_by_algorithm.plot(kind='bar', color='lightblue')
    plt.title('Avg Approx Time (s) by Algorithm')
    plt.ylabel('Avg Approx Time (s)')
    plt.xticks(rotation=45)
    plt.tight_layout()

    #αποθήκευση γραφήματος
    plt.savefig('approx_time_by_algorithm.png', dpi=300, bbox_inches='tight')
    plt.close()


#συνάρτηση που δημιουργεί σύνοψη στατιστικών    
def generate_summary_statistics(df):
    #εκτύπωση κεφαλίδας  
    print("=" * 60)
    print("📈 SUMMARY STATISTICS")
    print("=" * 60)

    #εύρεση καλύτερου αλγόριθμου ανά μετρική
    best_recall = df.loc[df['Recall@N'].idxmax()]   #καλύτερο Recall@N
    best_qps = df.loc[df['QPS'].idxmax()]           #καλύτερο QPS           
    best_af = df.loc[df['Avg AF'].idxmin()]         #καλύτερο AF (πλησιέστερο στο 1)

    #εκτύπωση αποτελεσμάτων 
    print(f"🎯 Best Recall@N: {best_recall['Recall@N']:.3f} ({best_recall['Algorithm']} - {best_recall.get('Parameters', 'N/A')})")
    print(f"⚡ Best QPS: {best_qps['QPS']:.1f} ({best_qps['Algorithm']} - {best_qps.get('Parameters', 'N/A')})")
    print(f"📏 Best AF (closest to 1): {best_af['Avg AF']:.3f} ({best_af['Algorithm']} - {best_af.get('Parameters', 'N/A')})")

    print("\n📊 Average by Algorithm:")
    avg_by_algo = df.groupby('Algorithm')[['Recall@N', 'QPS', 'Avg AF']].mean() #ομαδοποίηση και υπολογισμός μέσων όρων ανά αλγόριθμο
    print(avg_by_algo.round(3))                                                 #εκτύπωση στογγυλοποιημένων τιμών


def main_analysis():
    df = pd.read_csv('algorithm_comparison_table.csv')  #φόρτωση των δεδομένων από το csv αρχείο
    print("📁 Columns found:", df.columns.tolist())    #εκτύπωση διαθέσιμων στηλών     

    #κλήση συναρτήσεων για δημιουργία γραφημάτων    
    create_scatter_pairwise_comparisons(df)     #scatter plots
    create_comparison_plots(df)                 #βασικά bar charts
    create_bar_pairwise_comparisons(df)         #bar charts για ζεύγη μετρικών
    generate_summary_statistics(df)             #δημιουργία στατιστικής σύνοψης 

    #έλεγχος ύπαρξης απαραίτητων στηλών
    if {'Algorithm', 'Dataset', 'Recall@N', 'QPS', 'Avg AF', 'Avg Approx Time (s)'}.issubset(df.columns):
        #δημιουργία αναλυτικού πίνακα αποτελεσμάτων 
        summary_table = df.groupby(['Algorithm', 'Dataset']).agg({
            'Recall@N': 'mean',
            'QPS': 'mean',
            'Avg AF': 'mean',
            'Avg Approx Time (s)': 'mean'
        }).round(3) #στρογγυλοποίηση στα 3 δεκαδικά ψηφία

        summary_table.to_csv('detailed_summary.csv')    #αποθήκευση σε csv αρχείο
        print(f"\nSaved detailed summary to detailed_summary.csv")
    else:
        print("Κάποιες στήλες λείπουν — δεν δημιουργήθηκε detailed_summary.csv")    #μήνυμα σε περίπτωση που λείπουν στήλες 

#΄έλεγχος αν το script τρέχει απευθείας και κλήση της main συνάρτησης  
if __name__ == "__main__":
    main_analysis()