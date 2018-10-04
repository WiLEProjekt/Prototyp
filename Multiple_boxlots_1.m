A = [71.67586	,56.25862,	75.06207,	78.53103	,71.46897	,81.6931	95.63793	,72.61724	,89.48276	,87.4069	,70.5931,	89.38621,	72.76207,	82.26207]';
group = [ ones(size(A)) ];

figure

boxplot([A; ],group, 'Labels', {,'TCP_IPERF'})
title('Throughput (Downlink)');
ylabel('MBit/Sec');
xlabel('ORIGIN','3');
set(gca,'XTickLabel',{'A','B','C'})