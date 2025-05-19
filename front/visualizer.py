import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime

banlist = ['IDLE', 'Tmr Svc'] #ignoring threads

df = pd.read_csv('input.csv')
df.sort_values(by='time_ms', inplace=True)
df = df[~df['task'].isin(banlist)]

processes = df['task'].unique()

plt.figure(figsize=(12, 6))

for process in processes:
    process_data = df[df['task'] == process]
    for i in range(len(process_data) - 1):
        current = process_data.iloc[i]
        next_event = process_data.iloc[i + 1]
        if current['event'] == 'in':
            start_time = current['time_ms']
            end_time = next_event['time_ms']
            plt.barh(process, end_time - start_time, left=start_time, height=0.1, color='green', edgecolor='black')

plt.title('Гонка процессов')
plt.xlabel('Время (мс)')
plt.ylabel('Название процесса')
plt.grid(True, axis='x', linestyle='--', alpha=0.5)

plt.tight_layout() 
plt.yticks(processes)
plt.tight_layout()
plt.gcf().canvas.manager.set_window_title('Визуализатор гонки процессов')

now = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
plt.savefig(f"process_race_{now}.png", dpi=300)


plt.show()